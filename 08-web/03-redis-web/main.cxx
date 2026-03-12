#include <httplib.h>
#include <nlohmann/json.hpp>
#include <sw/redis++/redis++.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <format>
#include <fstream>
#include <mutex>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

namespace om
{

namespace redis = sw::redis;
using json      = nlohmann::json;

// Redis stream entry: (id, [(field_name, field_value), ...])
using stream_item =
    std::pair<std::string, std::vector<std::pair<std::string, std::string>>>;

constexpr std::string_view keyspace_all_devices = "__keyspace@0__:all_devices";
constexpr std::string_view keyspace_log_names   = "__keyspace@0__:log_names:";

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
    std::jthread                  worker_thread;

public:
    notification_center(std::shared_ptr<redis::Redis> r)
        : redis_client(std::move(r))
    {
    }

    ~notification_center()
    {
        if (worker_thread.joinable())
        {
            worker_thread.request_stop();
            try
            {
                redis_client->publish(std::string(keyspace_all_devices),
                                      "stop");
            }
            catch (...)
            {
            }
        }
    }

    void start()
    {
        try
        {
            redis_client->command(
                "CONFIG", "SET", "notify-keyspace-events", "KEA");
        }
        catch (const redis::Error& e)
        {
            std::println(stderr,
                         "error: Could not set notify-keyspace-events: {}",
                         e.what());
            std::terminate();
        }

        worker_thread = std::jthread(
            [this](std::stop_token stoken)
            {
                while (!stoken.stop_requested())
                {
                    try
                    {
                        auto sub = redis_client->subscriber();
                        sub.on_pmessage(
                            [this, stoken](std::string pattern,
                                           std::string channel,
                                           std::string msg)
                            {
                                if (stoken.stop_requested())
                                    return;

                                if (channel == keyspace_all_devices)
                                {
                                    devices_version++;
                                    cv_devices.notify_all();
                                }
                                else if (channel.starts_with(
                                             keyspace_log_names))
                                {
                                    std::string      device(channel.substr(
                                        keyspace_log_names.size()));
                                    std::scoped_lock lock(mtx);
                                    streams_version[device]++;
                                    cv_streams.notify_all();
                                }
                            });

                        sub.psubscribe(std::string(keyspace_all_devices));
                        sub.psubscribe(std::format("{}*", keyspace_log_names));

                        while (!stoken.stop_requested())
                        {
                            try
                            {
                                sub.consume();
                            }
                            catch (const redis::TimeoutError&)
                            {
                                continue;
                            }
                            catch (const redis::Error& e)
                            {
                                if (!stoken.stop_requested())
                                {
                                    std::println(
                                        stderr,
                                        "Redis subscriber consume error: {}",
                                        e.what());
                                }
                                break;
                            }
                        }
                    }
                    catch (const redis::Error& e)
                    {
                        if (!stoken.stop_requested())
                        {
                            std::println(stderr,
                                         "Redis subscriber error: {}. "
                                         "Reconnecting in 1s...",
                                         e.what());
                            std::this_thread::sleep_for(
                                std::chrono::seconds(1));
                        }
                    }
                }
            });
    }

    uint64_t get_devices_version() { return devices_version.load(); }

    uint64_t get_streams_version(const std::string& device)
    {
        std::scoped_lock lock(mtx);
        return streams_version[device];
    }

    bool wait_for_devices_change(uint64_t old_version, int timeout_sec = 15)
    {
        std::unique_lock lock(mtx);
        return cv_devices.wait_for(
            lock,
            std::chrono::seconds(timeout_sec),
            [this, old_version]
            { return devices_version.load() > old_version; });
    }

    bool wait_for_streams_change(const std::string& device,
                                 uint64_t           old_version,
                                 int                timeout_sec = 15)
    {
        std::unique_lock lock(mtx);
        return cv_streams.wait_for(
            lock,
            std::chrono::seconds(timeout_sec),
            [this, device, old_version]
            { return streams_version[device] > old_version; });
    }
};

struct server_config
{
    std::string redis_url    = "tcp://127.0.0.1:6379";
    std::string server_host  = "0.0.0.0";
    int         server_port  = 8080;
    std::string public_dir   = "./08-web/03-redis-web/public";
    int         socket_flags = 0;
};

server_config load_config(const std::string& filepath)
{
    server_config config;
    std::ifstream file(filepath);
    if (file.is_open())
    {
        try
        {
            json j;
            file >> j;
            if (j.contains("redis_url"))
                config.redis_url = j["redis_url"];
            if (j.contains("server_host"))
                config.server_host = j["server_host"];
            if (j.contains("server_port"))
                config.server_port = j["server_port"];
            if (j.contains("public_dir"))
                config.public_dir = j["public_dir"];
            if (j.contains("socket_flags"))
                config.socket_flags = j["socket_flags"];
        }
        catch (const std::exception& e)
        {
            std::println(stderr,
                         "Error parsing config file: {}\nUsing defaults.",
                         e.what());
        }
    }
    else
    {
        std::println(stderr,
                     "Could not open config file: {}\nUsing defaults.",
                     filepath);
    }
    return config;
}

// -----------------------------------------------------------------------------
// Провайдеры и обработчики (Handlers) для API
// -----------------------------------------------------------------------------

class devices_stream_provider
{
    std::shared_ptr<redis::Redis>        redis_client_;
    std::shared_ptr<notification_center> notifier_;

    json get_devices_json() const
    {
        std::vector<std::string> devices;
        redis_client_->smembers("all_devices",
                                std::inserter(devices, devices.begin()));

        json j = json::array();
        for (const auto& device : devices)
        {
            auto platform = redis_client_->hget(
                std::format("device_info:{}", device), "platform");
            j.push_back({ { "name", device },
                          { "platform", platform.value_or("Unknown") } });
        }
        return j;
    }

public:
    devices_stream_provider(std::shared_ptr<redis::Redis>        r,
                            std::shared_ptr<notification_center> n)
        : redis_client_(std::move(r))
        , notifier_(std::move(n))
    {
    }

    bool operator()(size_t offset, httplib::DataSink& sink) const
    {
        try
        {
            auto current_version = notifier_->get_devices_version();
            auto event_data      = std::format("event: init\ndata: {}\n\n",
                                          get_devices_json().dump());
            if (!sink.write(event_data.c_str(), event_data.size()))
                return false;

            while (true)
            {
                if (notifier_->wait_for_devices_change(current_version, 15))
                {
                    current_version = notifier_->get_devices_version();
                    auto update_event =
                        std::format("event: update\ndata: {}\n\n",
                                    get_devices_json().dump());
                    if (!sink.write(update_event.c_str(), update_event.size()))
                        return false;
                }
                else
                {
                    if (!sink.write(":\n\n", 3))
                        return false;
                }
            }
        }
        catch (const redis::Error& e)
        {
            auto error_msg =
                std::format("event: error\ndata: {}\n\n", e.what());
            sink.write(error_msg.c_str(), error_msg.size());
            return false;
        }
        return true;
    }
};

class devices_stream_handler
{
    std::shared_ptr<redis::Redis>        redis_client_;
    std::shared_ptr<notification_center> notifier_;

public:
    devices_stream_handler(std::shared_ptr<redis::Redis>        r,
                           std::shared_ptr<notification_center> n)
        : redis_client_(std::move(r))
        , notifier_(std::move(n))
    {
    }

    void operator()(const httplib::Request& req, httplib::Response& res) const
    {
        res.set_header("Content-Type", "text/event-stream");
        res.set_header("Cache-Control", "no-cache");
        res.set_header("Connection", "keep-alive");
        res.set_content_provider(
            "text/event-stream",
            devices_stream_provider{ redis_client_, notifier_ });
    }
};

class log_names_stream_provider
{
    std::shared_ptr<redis::Redis>        redis_client_;
    std::shared_ptr<notification_center> notifier_;
    std::string                          device_;

    json get_log_names_json() const
    {
        std::vector<std::string> names;
        redis_client_->smembers(std::format("log_names:{}", device_),
                                std::inserter(names, names.begin()));
        return json(names);
    }

public:
    log_names_stream_provider(std::shared_ptr<redis::Redis>        r,
                              std::shared_ptr<notification_center> n,
                              std::string                          device)
        : redis_client_(std::move(r))
        , notifier_(std::move(n))
        , device_(std::move(device))
    {
    }

    bool operator()(size_t offset, httplib::DataSink& sink) const
    {
        try
        {
            auto current_version = notifier_->get_streams_version(device_);
            auto event_data      = std::format("event: init\ndata: {}\n\n",
                                          get_log_names_json().dump());
            if (!sink.write(event_data.c_str(), event_data.size()))
                return false;

            while (true)
            {
                if (notifier_->wait_for_streams_change(
                        device_, current_version, 15))
                {
                    current_version = notifier_->get_streams_version(device_);
                    auto update_event =
                        std::format("event: update\ndata: {}\n\n",
                                    get_log_names_json().dump());
                    if (!sink.write(update_event.c_str(), update_event.size()))
                        return false;
                }
                else
                {
                    if (!sink.write(":\n\n", 3))
                        return false;
                }
            }
        }
        catch (const redis::Error& e)
        {
            auto error_msg =
                std::format("event: error\ndata: {}\n\n", e.what());
            sink.write(error_msg.c_str(), error_msg.size());
            return false;
        }
        return true;
    }
};

class log_names_stream_handler
{
    std::shared_ptr<redis::Redis>        redis_client_;
    std::shared_ptr<notification_center> notifier_;

public:
    log_names_stream_handler(std::shared_ptr<redis::Redis>        r,
                             std::shared_ptr<notification_center> n)
        : redis_client_(std::move(r))
        , notifier_(std::move(n))
    {
    }

    void operator()(const httplib::Request& req, httplib::Response& res) const
    {
        if (!req.has_param("device"))
        {
            res.status = 400;
            res.set_content(R"({"error": "Missing 'device' parameter"})",
                            "application/json");
            return;
        }

        auto device = req.get_param_value("device");

        res.set_header("Content-Type", "text/event-stream");
        res.set_header("Cache-Control", "no-cache");
        res.set_header("Connection", "keep-alive");

        res.set_content_provider(
            "text/event-stream",
            log_names_stream_provider{ redis_client_, notifier_, device });
    }
};

class logs_stream_provider
{
    std::shared_ptr<redis::Redis> redis_client_;
    std::string                   stream_key_;

    static json items_to_json(const auto& items)
    {
        auto to_entry = [](const stream_item& si)
        {
            const auto& [id, fields] = si;
            json entry;
            entry["id"] = id;
            for (const auto& [key, value] : fields)
                entry[key] = value;
            return entry;
        };

        return json(items | std::views::transform(to_entry) |
                    std::ranges::to<std::vector<json>>());
    }

public:
    logs_stream_provider(std::shared_ptr<redis::Redis> r, std::string key)
        : redis_client_(std::move(r))
        , stream_key_(std::move(key))
    {
    }

    bool operator()(size_t offset, httplib::DataSink& sink) const
    {
        std::string last_id = "0-0";

        try
        {
            std::vector<stream_item> stream_data;
            redis_client_->xrange(
                stream_key_, "-", "+", std::back_inserter(stream_data));

            if (!stream_data.empty())
            {
                auto event_data =
                    std::format("event: init\ndata: {}\n\n",
                                items_to_json(stream_data).dump());
                if (!sink.write(event_data.c_str(), event_data.size()))
                    return false;
                last_id = stream_data.back().first;
            }

            while (true)
            {
                std::vector<std::pair<std::string, std::vector<stream_item>>>
                    new_data;

                redis_client_->xread(
                    stream_key_, last_id, 15000, std::back_inserter(new_data));

                if (!new_data.empty() && !new_data[0].second.empty())
                {
                    const auto& items = new_data[0].second;
                    auto        event_data =
                        std::format("event: new_logs\ndata: {}\n\n",
                                    items_to_json(items).dump());
                    if (!sink.write(event_data.c_str(), event_data.size()))
                        return false;
                    last_id = items.back().first;
                }
                else
                {
                    if (!sink.write(":\n\n", 3))
                        return false;
                }
            }
        }
        catch (const redis::Error& e)
        {
            auto error_msg =
                std::format("event: error\ndata: {}\n\n", e.what());
            sink.write(error_msg.c_str(), error_msg.size());
            return false;
        }

        return true;
    }
};

class logs_stream_handler
{
    std::shared_ptr<redis::Redis> redis_client_;

public:
    explicit logs_stream_handler(std::shared_ptr<redis::Redis> r)
        : redis_client_(std::move(r))
    {
    }

    void operator()(const httplib::Request& req, httplib::Response& res) const
    {
        if (!req.has_param("device") || !req.has_param("log_name"))
        {
            res.status = 400;
            res.set_content(
                R"({"error": "Missing 'device' or 'log_name' parameter"})",
                "application/json");
            return;
        }

        auto stream_key = std::format("log:{}:{}",
                                      req.get_param_value("device"),
                                      req.get_param_value("log_name"));

        res.set_header("Content-Type", "text/event-stream");
        res.set_header("Cache-Control", "no-cache");
        res.set_header("Connection", "keep-alive");
        res.set_content_provider(
            "text/event-stream",
            logs_stream_provider{ redis_client_, stream_key });
    }
};

class logs_download_provider
{
    std::shared_ptr<redis::Redis> redis_client_;
    std::string                   stream_key_;
    std::shared_ptr<std::string>  last_id_;

public:
    logs_download_provider(std::shared_ptr<redis::Redis> r,
                           std::string                   stream_key)
        : redis_client_(std::move(r))
        , stream_key_(std::move(stream_key))
        , last_id_(std::make_shared<std::string>("-"))
    {
    }

    bool operator()(size_t offset, httplib::DataSink& sink)
    {
        constexpr size_t batch_size = 5000;

        try
        {
            std::vector<stream_item> stream_data;

            std::string start_id = (*last_id_ == "-") ? "-" : "(" + *last_id_;

            redis_client_->xrange(stream_key_,
                                  start_id,
                                  "+",
                                  batch_size,
                                  std::back_inserter(stream_data));

            if (stream_data.empty())
            {
                sink.done();
                return false;
            }

            std::string chunk;
            chunk.reserve(stream_data.size() * 100);

            for (const auto& [id, fields] : stream_data)
            {
                for (const auto& [key, value] : fields)
                {
                    if (key == "message")
                    {
                        chunk += value;
                        chunk += '\n';
                        break;
                    }
                }
                *last_id_ = id;
            }

            if (!chunk.empty())
            {
                if (!sink.write(chunk.c_str(), chunk.size()))
                    return false;
            }

            if (stream_data.size() < batch_size)
            {
                sink.done();
                return false;
            }

            return true;
        }
        catch (const redis::Error& e)
        {
            std::println(stderr, "Redis download error: {}", e.what());
            sink.done();
            return false;
        }
    }
};

class logs_download_handler
{
    std::shared_ptr<redis::Redis> redis_client_;

public:
    explicit logs_download_handler(std::shared_ptr<redis::Redis> r)
        : redis_client_(std::move(r))
    {
    }

    void operator()(const httplib::Request& req, httplib::Response& res) const
    {
        if (!req.has_param("device") || !req.has_param("log_name"))
        {
            res.status = 400;
            res.set_content("Missing 'device' or 'log_name' parameter",
                            "text/plain");
            return;
        }

        auto device     = req.get_param_value("device");
        auto log_name   = req.get_param_value("log_name");
        auto stream_key = std::format("log:{}:{}", device, log_name);

        res.set_header("Content-Disposition",
                       std::format("attachment; filename=\"{}\"", log_name));

        res.set_chunked_content_provider(
            "text/plain", logs_download_provider{ redis_client_, stream_key });
    }
};

} // namespace om

int main(int argc, char* argv[])
{
    namespace redis = sw::redis;

    om::server_config config;
    if (argc > 1)
    {
        config = om::load_config(argv[1]);
    }
    else
    {
        std::println("Usage: {} [config.json]\n"
                     "No config file provided, using default configuration.",
                     argv[0]);
    }

    auto redis_client = std::make_shared<redis::Redis>(config.redis_url);

    auto notifier = std::make_shared<om::notification_center>(redis_client);
    notifier->start();

    httplib::Server svr;

    // Отключаем SO_REUSEPORT (если он есть), оставляем только SO_REUSEADDR,
    // чтобы при попытке запустить второй сервер на том же порту мы получали
    // ошибку.
    svr.set_socket_options(
        [](auto sock)
        {
#ifndef _WIN32
            int yes = 1;
            setsockopt(sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       reinterpret_cast<const void*>(&yes),
                       sizeof(yes));
#ifdef SO_REUSEPORT
            int no = 0;
            setsockopt(sock,
                       SOL_SOCKET,
                       SO_REUSEPORT,
                       reinterpret_cast<const void*>(&no),
                       sizeof(no));
#endif
#endif
        });

    svr.set_mount_point("/", config.public_dir);

    svr.Get("/api/devices/stream",
            om::devices_stream_handler{ redis_client, notifier });

    svr.Get("/api/log_names/stream",
            om::log_names_stream_handler{ redis_client, notifier });

    svr.Get("/api/logs/stream", om::logs_stream_handler{ redis_client });

    svr.Get("/api/logs/download", om::logs_download_handler{ redis_client });

    std::println("Starting web server on http://{}:{}",
                 config.server_host,
                 config.server_port);
    std::println("Serving static files from {}", config.public_dir);

    if (!svr.listen(
            config.server_host, config.server_port, config.socket_flags))
    {
        std::println(stderr,
                     "Failed to start server on {}:{}",
                     config.server_host,
                     config.server_port);
        return 1;
    }

    return 0;
}
