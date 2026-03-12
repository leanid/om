#include <httplib.h>
#include <nlohmann/json.hpp>
#include <sw/redis++/redis++.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace om
{

namespace redis = sw::redis;
using json      = nlohmann::json;

// Вспомогательная функция для форматирования времени (больше не используется,
// но оставим на всякий случай)
std::string format_timestamp(const std::string& ts_str)
{
    try
    {
        long long                             ms = std::stoll(ts_str);
        std::chrono::system_clock::time_point tp{ std::chrono::milliseconds(
            ms) };
        std::time_t        t  = std::chrono::system_clock::to_time_t(tp);
        std::tm*           tm = std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(tm, "%H:%M:%S") << "." << std::setfill('0')
            << std::setw(3) << (ms % 1000);
        return oss.str();
    }
    catch (...)
    {
        return ts_str;
    }
}

// Класс для централизованного отслеживания изменений в Redis через Keyspace
// Notifications
class notification_center
{
    std::mutex              mtx;
    std::condition_variable cv_devices;
    std::condition_variable cv_streams;

    std::atomic<uint64_t>                     devices_version{ 0 };
    std::unordered_map<std::string, uint64_t> streams_version;

    std::shared_ptr<redis::Redis> redis_client;

public:
    notification_center(std::shared_ptr<redis::Redis> r)
        : redis_client(r)
    {
    }

    void start()
    {
        // Включаем Keyspace Notifications (K: keyspace events, E: keyevent
        // events, s: set commands)
        try
        {
            redis_client->command(
                "CONFIG", "SET", "notify-keyspace-events", "KEA");
        }
        catch (const redis::Error& e)
        {
            std::cerr << "error: Could not set notify-keyspace-events: "
                      << e.what() << std::endl;
            std::terminate();
        }

        std::thread(
            [this]()
            {
                while (true)
                {
                    try
                    {
                        auto sub = redis_client->subscriber();
                        sub.on_pmessage(
                            [this](std::string pattern,
                                   std::string channel,
                                   std::string msg)
                            {
                                if (channel == "__keyspace@0__:all_devices")
                                {
                                    devices_version++;
                                    cv_devices.notify_all();
                                }
                                else if (channel.find(
                                             "__keyspace@0__:streams:") == 0)
                                {
                                    std::string device = channel.substr(
                                        std::string("__keyspace@0__:streams:")
                                            .length());
                                    std::lock_guard<std::mutex> lock(mtx);
                                    streams_version[device]++;
                                    cv_streams.notify_all();
                                }
                            });

                        sub.psubscribe("__keyspace@0__:all_devices");
                        sub.psubscribe("__keyspace@0__:streams:*");

                        while (true)
                        {
                            try
                            {
                                sub.consume();
                            }
                            catch (const redis::TimeoutError&)
                            {
                                // Это нормально, таймаут ожидания, продолжаем
                                // слушать
                                continue;
                            }
                            catch (const redis::Error& e)
                            {
                                std::cerr << "Redis subscriber consume error: "
                                          << e.what() << std::endl;
                                break; // Выходим из внутреннего цикла, чтобы
                                       // переподключиться
                            }
                        }
                    }
                    catch (const redis::Error& e)
                    {
                        std::cerr << "Redis subscriber error: " << e.what()
                                  << ". Reconnecting in 1s..." << std::endl;
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                }
            })
            .detach();
    }

    uint64_t get_devices_version() { return devices_version.load(); }

    uint64_t get_streams_version(const std::string& device)
    {
        std::lock_guard<std::mutex> lock(mtx);
        return streams_version[device];
    }

    bool wait_for_devices_change(uint64_t old_version, int timeout_sec = 15)
    {
        std::unique_lock<std::mutex> lock(mtx);
        return cv_devices.wait_for(
            lock,
            std::chrono::seconds(timeout_sec),
            [this, old_version]()
            { return devices_version.load() > old_version; });
    }

    bool wait_for_streams_change(const std::string& device,
                                 uint64_t           old_version,
                                 int                timeout_sec = 15)
    {
        std::unique_lock<std::mutex> lock(mtx);
        return cv_streams.wait_for(
            lock,
            std::chrono::seconds(timeout_sec),
            [this, device, old_version]()
            { return streams_version[device] > old_version; });
    }
};

} // namespace om

int main()
{
    namespace redis = sw::redis;
    using json      = nlohmann::json;

    // Подключаемся к Redis
    std::string connection_string = "tcp://127.0.0.1:6379";
    auto redis_client = std::make_shared<redis::Redis>(connection_string);

    // Запускаем центр уведомлений
    auto notifier = std::make_shared<om::notification_center>(redis_client);
    notifier->start();

    httplib::Server svr;

    // Раздаем статические файлы из папки public
    const std::string public_dir = "./08-web/03-redis-web/public";
    svr.set_mount_point("/", public_dir);

    // API: Получить список всех устройств с их платформами (SSE)
    svr.Get(
        "/api/devices/stream",
        [&redis_client, notifier](const httplib::Request& req,
                                  httplib::Response&      res)
        {
            res.set_header("Content-Type", "text/event-stream");
            res.set_header("Cache-Control", "no-cache");
            res.set_header("Connection", "keep-alive");

            res.set_content_provider(
                "text/event-stream",
                [&redis_client, &notifier](size_t             offset,
                                           httplib::DataSink& sink)
                {
                    try
                    {
                        // Функция для получения текущего списка устройств
                        auto get_devices_json = [&redis_client]()
                        {
                            std::vector<std::string> devices;
                            redis_client->smembers(
                                "all_devices",
                                std::inserter(devices, devices.begin()));

                            json j = json::array();
                            for (const auto& device : devices)
                            {
                                auto platform_opt = redis_client->hget(
                                    "device_info:" + device, "platform");
                                std::string platform =
                                    platform_opt ? *platform_opt : "Unknown";
                                j.push_back({ { "name", device },
                                              { "platform", platform } });
                            }
                            return j;
                        };

                        // Отправляем начальный список
                        uint64_t current_version =
                            notifier->get_devices_version();
                        json        initial_devices = get_devices_json();
                        std::string event_data =
                            "event: init\ndata: " + initial_devices.dump() +
                            "\n\n";
                        if (!sink.write(event_data.c_str(), event_data.size()))
                            return false;

                        while (true)
                        {
                            // Ожидаем изменений через condition_variable (без
                            // активного sleep/polling)
                            bool changed = notifier->wait_for_devices_change(
                                current_version, 15);

                            if (changed)
                            {
                                current_version =
                                    notifier->get_devices_version();
                                json current_devices = get_devices_json();
                                std::string update_event =
                                    "event: update\ndata: " +
                                    current_devices.dump() + "\n\n";
                                if (!sink.write(update_event.c_str(),
                                                update_event.size()))
                                    return false;
                            }
                            else
                            {
                                // Ping для поддержания соединения
                                if (!sink.write(":\n\n", 3))
                                    return false;
                            }
                        }
                    }
                    catch (const redis::Error& e)
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

    // API: Получить список стримов для конкретного устройства (SSE)
    svr.Get(
        "/api/streams/stream",
        [&redis_client, notifier](const httplib::Request& req,
                                  httplib::Response&      res)
        {
            if (!req.has_param("device"))
            {
                res.status = 400;
                res.set_content(R"({"error": "Missing 'device' parameter"})",
                                "application/json");
                return;
            }

            std::string device = req.get_param_value("device");

            res.set_header("Content-Type", "text/event-stream");
            res.set_header("Cache-Control", "no-cache");
            res.set_header("Connection", "keep-alive");

            res.set_content_provider(
                "text/event-stream",
                [&redis_client, notifier, device](size_t             offset,
                                                  httplib::DataSink& sink)
                {
                    try
                    {
                        auto get_streams_json = [&redis_client, &device]()
                        {
                            std::vector<std::string> streams;
                            redis_client->smembers(
                                "streams:" + device,
                                std::inserter(streams, streams.begin()));
                            return json(streams);
                        };

                        uint64_t current_version =
                            notifier->get_streams_version(device);
                        json        initial_streams = get_streams_json();
                        std::string event_data =
                            "event: init\ndata: " + initial_streams.dump() +
                            "\n\n";
                        if (!sink.write(event_data.c_str(), event_data.size()))
                            return false;

                        while (true)
                        {
                            bool changed = notifier->wait_for_streams_change(
                                device, current_version, 15);

                            if (changed)
                            {
                                current_version =
                                    notifier->get_streams_version(device);
                                json current_streams = get_streams_json();
                                std::string update_event =
                                    "event: update\ndata: " +
                                    current_streams.dump() + "\n\n";
                                if (!sink.write(update_event.c_str(),
                                                update_event.size()))
                                    return false;
                            }
                            else
                            {
                                // Ping
                                if (!sink.write(":\n\n", 3))
                                    return false;
                            }
                        }
                    }
                    catch (const redis::Error& e)
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

    // API: Получить логи для конкретного устройства (SSE)
    svr.Get(
        "/api/logs/stream",
        [&redis_client](const httplib::Request& req, httplib::Response& res)
        {
            if (!req.has_param("device") || !req.has_param("stream"))
            {
                res.status = 400;
                res.set_content(
                    R"({"error": "Missing 'device' or 'stream' parameter"})",
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
                [&redis_client, stream_key](size_t             offset,
                                            httplib::DataSink& sink)
                {
                    // Изначально читаем все существующие логи
                    std::string last_id = "0-0";

                    try
                    {
                        // Сначала отправляем все старые логи
                        using item_type = std::pair<
                            std::string,
                            std::vector<std::pair<std::string, std::string>>>;
                        std::vector<item_type> stream_data;

                        redis_client->xrange(stream_key,
                                             "-",
                                             "+",
                                             std::back_inserter(stream_data));

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
                                std::pair<std::string, std::vector<item_type>>>
                                new_data;

                            // Блокирующее чтение (таймаут 15 секунд, чтобы
                            // проверять закрытие соединения и отправлять ping)
                            redis_client->xread(stream_key,
                                                last_id,
                                                15000,
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
                    catch (const redis::Error& e)
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
        [&redis_client](const httplib::Request& req, httplib::Response& res)
        {
            if (!req.has_param("device") || !req.has_param("stream"))
            {
                res.status = 400;
                res.set_content("Missing 'device' or 'stream' parameter",
                                "text/plain");
                return;
            }

            std::string device     = req.get_param_value("device");
            std::string stream     = req.get_param_value("stream");
            std::string stream_key = "log:" + device + ":" + stream;

            // Устанавливаем заголовки для скачивания файла
            res.set_header("Content-Disposition",
                           "attachment; filename=\"" + stream + "\"");

            // Используем chunked передачу данных (Chunked Transfer Encoding)
            res.set_chunked_content_provider(
                "text/plain",
                [redis_client,
                 stream_key,
                 last_id = std::make_shared<std::string>("-")](
                    size_t offset, httplib::DataSink& sink) mutable
                {
                    const int batch_size =
                        5000; // Читаем по 5000 записей за раз

                    try
                    {
                        using item_type = std::pair<
                            std::string,
                            std::vector<std::pair<std::string, std::string>>>;
                        std::vector<item_type> stream_data;

                        // Читаем батч данных (XRANGE stream_key last_id + COUNT
                        // batch_size) Если last_id != "-", используем синтаксис
                        // (last_id для исключения уже прочитанного элемента
                        std::string start_id =
                            (*last_id == "-") ? "-" : "(" + *last_id;

                        redis_client->xrange(stream_key,
                                             start_id,
                                             "+",
                                             batch_size,
                                             std::back_inserter(stream_data));

                        if (stream_data.empty())
                        {
                            sink.done();
                            return false; // Данные закончились, завершаем
                        }

                        std::string chunk;
                        // Резервируем память (примерно 100 байт на строку),
                        // чтобы избежать лишних аллокаций
                        chunk.reserve(stream_data.size() * 100);

                        for (size_t i = 0; i < stream_data.size(); ++i)
                        {
                            const auto& item = stream_data[i];

                            for (const auto& field : item.second)
                            {
                                if (field.first == "message")
                                {
                                    chunk += field.second;
                                    chunk += '\n';
                                    break;
                                }
                            }
                            *last_id = item.first; // Обновляем ID для
                                                   // следующего запроса
                        }

                        // Отправляем чанк браузеру
                        if (!chunk.empty())
                        {
                            if (!sink.write(chunk.c_str(), chunk.size()))
                            {
                                return false; // Ошибка записи
                            }
                        }

                        // Если мы получили меньше записей, чем просили, значит
                        // это был последний батч
                        if (stream_data.size() <
                            static_cast<size_t>(batch_size))
                        {
                            sink.done();
                            return false;
                        }

                        return true; // Продолжаем передачу
                    }
                    catch (const redis::Error& e)
                    {
                        std::cerr << "Redis download error: " << e.what()
                                  << std::endl;
                        sink.done();
                        return false;
                    }
                });
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
