#include <httplib.h>
#include <nlohmann/json.hpp>
#include <sw/redis++/redis++.h>

#include <iostream>
#include <string>
#include <vector>

using namespace sw::redis;
using json = nlohmann::json;

int main()
{
    // Подключаемся к Redis
    std::string connection_string = "tcp://127.0.0.1:6379";
    auto        redis             = std::make_shared<Redis>(connection_string);

    httplib::Server svr;

    // Раздаем статические файлы из папки public
    const std::string public_dir = "./08-web/03-redis-web/public";
    svr.set_mount_point("/", public_dir);

    // API: Получить список всех устройств
    svr.Get("/api/devices",
            [&redis](const httplib::Request& req, httplib::Response& res)
            {
                std::vector<std::string> devices;
                try
                {
                    redis->smembers("all_devices",
                                    std::inserter(devices, devices.begin()));

                    json j = devices;
                    res.set_content(j.dump(), "application/json");
                }
                catch (const Error& e)
                {
                    json j     = { { "error", e.what() } };
                    res.status = 500;
                    res.set_content(j.dump(), "application/json");
                }
            });

    // API: Получить логи для конкретного устройства
    svr.Get(
        "/api/logs",
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

            try
            {
                // Читаем все записи из стрима
                // XRANGE device - +
                using Item =
                    std::pair<std::string,
                              std::vector<std::pair<std::string, std::string>>>;
                std::vector<Item> stream_data;

                redis->xrange(
                    device, "-", "+", std::back_inserter(stream_data));

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
                }

                res.set_content(logs.dump(), "application/json");
            }
            catch (const Error& e)
            {
                json j     = { { "error", e.what() } };
                res.status = 500;
                res.set_content(j.dump(), "application/json");
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
