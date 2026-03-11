#include <httplib.h>
#include <nlohmann/json.hpp>
#include <sw/redis++/redis++.h>

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace sw::redis;
using json = nlohmann::json;

// Вспомогательная функция для форматирования времени
std::string format_timestamp(const std::string& ts_str) {
    try {
        long long ms = std::stoll(ts_str);
        std::chrono::system_clock::time_point tp{std::chrono::milliseconds(ms)};
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm* tm = std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(tm, "%H:%M:%S") << "." << std::setfill('0') << std::setw(3) << (ms % 1000);
        return oss.str();
    } catch (...) {
        return ts_str;
    }
}

int main()
{
    // Подключаемся к Redis
    std::string connection_string = "tcp://127.0.0.1:6379";
    auto        redis             = std::make_shared<Redis>(connection_string);

    httplib::Server svr;

    // Раздаем статические файлы из папки public
    const std::string public_dir = "./08-web/03-redis-web/public";
    svr.set_mount_point("/", public_dir);

    // API: Получить список всех устройств с их платформами
    svr.Get("/api/devices",
            [&redis](const httplib::Request& req, httplib::Response& res)
            {
                std::vector<std::string> devices;
                try
                {
                    redis->smembers("all_devices",
                                    std::inserter(devices, devices.begin()));

                    json j = json::array();
                    
                    // Для каждого устройства получаем его платформу
                    for (const auto& device : devices) {
                        auto platform_opt = redis->hget("device_info:" + device, "platform");
                        std::string platform = platform_opt ? *platform_opt : "Unknown";
                        
                        j.push_back({
                            {"name", device},
                            {"platform", platform}
                        });
                    }

                    res.set_content(j.dump(), "application/json");
                }
                catch (const Error& e)
                {
                    json j     = { { "error", e.what() } };
                    res.status = 500;
                    res.set_content(j.dump(), "application/json");
                }
            });

    // API: Получить список стримов для конкретного устройства
    svr.Get("/api/streams",
            [&redis](const httplib::Request& req, httplib::Response& res)
            {
                if (!req.has_param("device"))
                {
                    res.status = 400;
                    res.set_content(R"({"error": "Missing 'device' parameter"})",
                                    "application/json");
                    return;
                }

                std::string device = req.get_param_value("device");
                std::vector<std::string> streams;
                try
                {
                    redis->smembers("streams:" + device,
                                    std::inserter(streams, streams.begin()));

                    json j = streams;
                    res.set_content(j.dump(), "application/json");
                }
                catch (const Error& e)
                {
                    json j     = { { "error", e.what() } };
                    res.status = 500;
                    res.set_content(j.dump(), "application/json");
                }
            });

    // API: Получить логи для конкретного устройства (SSE)
    svr.Get(
        "/api/logs/stream",
        [&redis](const httplib::Request& req, httplib::Response& res)
        {
            if (!req.has_param("device") || !req.has_param("stream"))
            {
                res.status = 400;
                res.set_content(R"({"error": "Missing 'device' or 'stream' parameter"})",
                                "application/json");
                return;
            }

            std::string device = req.get_param_value("device");
            std::string stream = req.get_param_value("stream");
            
            // Формируем полный ключ стрима
            std::string stream_key = "log:" + device + ":" + stream;

            // Настраиваем заголовки для SSE(Server Side Event)
            res.set_header("Content-Type", "text/event-stream");
            res.set_header("Cache-Control", "no-cache");
            res.set_header("Connection", "keep-alive");

            // Используем chunked передачу данных
            res.set_content_provider(
                "text/event-stream",
                [&redis, stream_key](size_t offset, httplib::DataSink& sink)
                {
                    // Изначально читаем все существующие логи
                    std::string last_id = "0-0";

                    try
                    {
                        // Сначала отправляем все старые логи
                        using Item = std::pair<
                            std::string,
                            std::vector<std::pair<std::string, std::string>>>;
                        std::vector<Item> stream_data;

                        redis->xrange(
                            stream_key, "-", "+", std::back_inserter(stream_data));

                        if (!stream_data.empty())
                        {
                            json logs = json::array();
                            for (const auto& item : stream_data)
                            {
                                json log_entry;
                                log_entry["id"] = item.first;
                                for (const auto& field : item.second)
                                {
                                    log_entry[field.first] = field.second;
                                }
                                logs.push_back(log_entry);
                                last_id = item.first; // Запоминаем последний ID
                            }

                            // Отправляем начальный батч логов
                            std::string event_data =
                                "event: init\ndata: " + logs.dump() + "\n\n";
                            if (!sink.write(event_data.c_str(),
                                            event_data.size()))
                                return false;
                        }

                        // Бесконечный цикл чтения новых логов (XREAD BLOCK)
                        while (true)
                        {
                            std::vector<
                                std::pair<std::string, std::vector<Item>>>
                                new_data;

                            // Блокирующее чтение (таймаут 1 секунда, чтобы
                            // проверять закрытие соединения)
                            redis->xread(stream_key,
                                         last_id,
                                         1000,
                                         std::back_inserter(new_data));

                            if (!new_data.empty() &&
                                !new_data[0].second.empty())
                            {
                                json logs = json::array();
                                for (const auto& item : new_data[0].second)
                                {
                                    json log_entry;
                                    log_entry["id"] = item.first;
                                    for (const auto& field : item.second)
                                    {
                                        log_entry[field.first] = field.second;
                                    }
                                    logs.push_back(log_entry);
                                    last_id = item.first; // Обновляем ID
                                }

                                // Отправляем новые логи
                                std::string event_data =
                                    "event: new_logs\ndata: " + logs.dump() +
                                    "\n\n";
                                if (!sink.write(event_data.c_str(),
                                                event_data.size()))
                                    return false;
                            }
                            else
                            {
                                // Отправляем ping (комментарий), чтобы
                                // поддерживать соединение живым
                                if (!sink.write(":\n\n", 3))
                                    return false;
                            }
                        }
                    }
                    catch (const Error& e)
                    {
                        std::string error_msg =
                            std::string("event: error\ndata: ") + e.what() +
                            "\n\n";
                        sink.write(error_msg.c_str(), error_msg.size());
                        return false;
                    }

                    return true;
                });
        });

    // API: Скачать лог целиком как текстовый файл
    svr.Get(
        "/api/logs/download",
        [&redis](const httplib::Request& req, httplib::Response& res)
        {
            if (!req.has_param("device") || !req.has_param("stream"))
            {
                res.status = 400;
                res.set_content("Missing 'device' or 'stream' parameter", "text/plain");
                return;
            }

            std::string device = req.get_param_value("device");
            std::string stream = req.get_param_value("stream");
            std::string stream_key = "log:" + device + ":" + stream;

            try
            {
                using Item = std::pair<std::string, std::vector<std::pair<std::string, std::string>>>;
                std::vector<Item> stream_data;

                redis->xrange(stream_key, "-", "+", std::back_inserter(stream_data));

                std::ostringstream out;
                for (const auto& item : stream_data)
                {
                    std::string ts, lvl, msg;
                    for (const auto& field : item.second)
                    {
                        if (field.first == "timestamp") ts = field.second;
                        else if (field.first == "level") lvl = field.second;
                        else if (field.first == "message") msg = field.second;
                    }
                    out << "[" << format_timestamp(ts) << "] " << lvl << " " << msg << "\n";
                }

                // Устанавливаем заголовки для скачивания файла
                res.set_header("Content-Disposition", "attachment; filename=\"" + stream + "\"");
                res.set_content(out.str(), "text/plain");
            }
            catch (const Error& e)
            {
                res.status = 500;
                res.set_content(std::string("Redis error: ") + e.what(), "text/plain");
            }
        });

    int port = 8080;
    std::cout << "Starting web server on http://localhost:" << port
              << std::endl;
    std::cout << "Serving static files from " << public_dir << std::endl;

    if (!svr.listen("0.0.0.0", port))
    {
        std::cerr << "Failed to start server on port " << port << std::endl;
        return 1;
    }

    return 0;
}
