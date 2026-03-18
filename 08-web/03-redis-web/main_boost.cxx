// Functionally equivalent to main.cxx but built entirely on Boost (Beast +
// Asio + Redis) using C++20 coroutines. A single multiplexed
// boost::redis::connection replaces the redis-plus-plus client pool.

#include <boost/redis/src.hpp>

#include <boost/asio.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/redis/adapter/adapt.hpp>
#include <boost/redis/connection.hpp>
#include <boost/redis/request.hpp>
#include <boost/redis/resp3/node.hpp>
#include <boost/redis/response.hpp>

#include <nlohmann/json.hpp>

#ifndef _WIN32
#include <sys/socket.h>
#endif

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

namespace beast  = boost::beast;
namespace http   = beast::http;
namespace net    = boost::asio;
namespace bredis = boost::redis;
using tcp        = net::ip::tcp;
using json       = nlohmann::json;

using stream_item =
    std::pair<std::string, std::vector<std::pair<std::string, std::string>>>;

constexpr std::string_view keyspace_all_devices = "__keyspace@0__:all_devices";
constexpr std::string_view keyspace_log_names   = "__keyspace@0__:log_names:";

// =============================================================================
// Generic-response helpers — walk the RESP3 node tree returned by Boost.Redis
// =============================================================================

std::vector<stream_item> parse_stream_entries(
    const std::vector<bredis::resp3::node>& nodes, std::size_t& pos)
{
    std::vector<stream_item> result;
    if (pos >= nodes.size() ||
        nodes[pos].data_type == bredis::resp3::type::null)
    {
        return result;
    }
    auto entry_count = nodes[pos].aggregate_size;
    ++pos;

    for (std::size_t e = 0; e < entry_count && pos < nodes.size(); ++e)
    {
        ++pos; // entry array header [id, fields]

        std::string id(nodes[pos].value);
        ++pos;

        std::vector<std::pair<std::string, std::string>> fields;
        auto field_count = nodes[pos].aggregate_size;
        ++pos;

        for (std::size_t f = 0; f + 1 < field_count; f += 2)
        {
            fields.emplace_back(std::string(nodes[pos].value),
                                std::string(nodes[pos + 1].value));
            pos += 2;
        }

        result.emplace_back(std::move(id), std::move(fields));
    }
    return result;
}

std::vector<stream_item> parse_xrange(const bredis::generic_response& resp)
{
    if (!resp.has_value() || resp.value().empty())
    {
        return {};
    }
    std::size_t pos = 0;
    return parse_stream_entries(resp.value(), pos);
}

std::vector<std::pair<std::string, std::vector<stream_item>>> parse_xread(
    const bredis::generic_response& resp)
{
    std::vector<std::pair<std::string, std::vector<stream_item>>> result;
    if (!resp.has_value() || resp.value().empty() ||
        resp.value()[0].data_type == bredis::resp3::type::null)
    {
        return result;
    }

    auto&       nodes        = resp.value();
    std::size_t pos          = 0;
    auto        stream_count = nodes[pos].aggregate_size;
    ++pos;

    for (std::size_t s = 0; s < stream_count && pos < nodes.size(); ++s)
    {
        ++pos; // stream array header [name, entries]
        std::string name(nodes[pos].value);
        ++pos;
        auto entries = parse_stream_entries(nodes, pos);
        result.emplace_back(std::move(name), std::move(entries));
    }
    return result;
}

std::unordered_map<std::string, std::string> parse_hgetall(
    const bredis::generic_response& resp)
{
    std::unordered_map<std::string, std::string> result;
    if (!resp.has_value() || resp.value().empty())
    {
        return result;
    }
    auto& nodes = resp.value();
    // RESP3 HGETALL returns a map: aggregate with paired elements.
    // In node representation: map header, then key/value pairs at depth+1.
    std::size_t pos   = 0;
    auto        count = nodes[pos].aggregate_size *
                 bredis::resp3::element_multiplicity(nodes[pos].data_type);
    ++pos;
    for (std::size_t i = 0; i + 1 < count && pos + 1 < nodes.size(); i += 2)
    {
        result[std::string(nodes[pos].value)] =
            std::string(nodes[pos + 1].value);
        pos += 2;
    }
    return result;
}

std::vector<std::string> parse_smembers(const bredis::generic_response& resp)
{
    std::vector<std::string> result;
    if (!resp.has_value() || resp.value().empty())
    {
        return result;
    }
    auto& nodes = resp.value();
    auto  count = nodes[0].aggregate_size;
    for (std::size_t i = 1; i <= count && i < nodes.size(); ++i)
    {
        result.emplace_back(nodes[i].value);
    }
    return result;
}

// =============================================================================
// Redis URL parser
// =============================================================================

struct redis_url_parts
{
    std::string host     = "127.0.0.1";
    std::string port     = "6379";
    std::string username = "default";
    std::string password;
};

redis_url_parts parse_redis_url(const std::string& url)
{
    redis_url_parts parts;
    std::string     s = url;

    if (auto pos = s.find("://"); pos != std::string::npos)
    {
        s = s.substr(pos + 3);
    }

    if (auto at = s.find('@'); at != std::string::npos)
    {
        auto cred = s.substr(0, at);
        s         = s.substr(at + 1);

        if (auto colon = cred.find(':'); colon != std::string::npos)
        {
            parts.username = cred.substr(0, colon);
            parts.password = cred.substr(colon + 1);
        }
        else
        {
            parts.password = cred;
        }
    }

    if (auto colon = s.find(':'); colon != std::string::npos)
    {
        parts.host = s.substr(0, colon);
        parts.port = s.substr(colon + 1);
    }
    else if (!s.empty())
    {
        parts.host = s;
    }

    return parts;
}

// =============================================================================
// Notification center — keyspace-notification watcher via Boost.Redis
// Uses the connection's async_receive for PSUBSCRIBE push messages.
// =============================================================================

class notification_center
{
    std::mutex              mtx_;
    std::condition_variable cv_devices_;
    std::condition_variable cv_streams_;
    std::condition_variable cv_any_;

    std::atomic<uint64_t>                     devices_version_{ 0 };
    std::atomic<uint64_t>                     any_version_{ 0 };
    std::unordered_map<std::string, uint64_t> streams_version_;

    net::io_context    ioc_;
    bredis::connection conn_;
    bredis::config     redis_cfg_;
    std::jthread       ioc_thread_;

    void process_push(const bredis::generic_response& resp)
    {
        if (!resp.has_value())
        {
            return;
        }
        auto& nodes = resp.value();
        // Push message: [push_header, "pmessage", pattern, channel, data]
        // In RESP3 node form: push aggregate at depth 0, children at depth 1.
        if (nodes.size() < 4)
        {
            return;
        }

        auto& kind = nodes[1].value;
        if (kind != "pmessage" || nodes.size() < 5)
        {
            return;
        }

        auto& channel = nodes[3].value;

        if (channel == keyspace_all_devices)
        {
            devices_version_++;
            any_version_++;
            cv_devices_.notify_all();
            cv_any_.notify_all();
        }
        else if (channel.starts_with(keyspace_log_names))
        {
            std::string      device(channel.substr(keyspace_log_names.size()));
            std::scoped_lock lock(mtx_);
            streams_version_[device]++;
            any_version_++;
            cv_streams_.notify_all();
            cv_any_.notify_all();
        }
    }

    net::awaitable<void> subscriber_loop()
    {
        bredis::generic_response resp;
        conn_.set_receive_response(resp);

        while (true)
        {
            boost::system::error_code ec;
            co_await conn_.async_receive(
                net::redirect_error(net::deferred, ec));
            if (ec)
            {
                break;
            }
            process_push(resp);
            bredis::consume_one(resp);
        }
    }

public:
    explicit notification_center(const redis_url_parts& url)
        : conn_(ioc_)
    {
        redis_cfg_.addr                    = { url.host, url.port };
        redis_cfg_.username                = url.username;
        redis_cfg_.password                = url.password;
        redis_cfg_.reconnect_wait_interval = std::chrono::seconds(1);
        redis_cfg_.health_check_interval   = std::chrono::seconds(5);
    }

    ~notification_center()
    {
        conn_.cancel(bredis::operation::all);
        ioc_.stop();
    }

    void start()
    {
        net::co_spawn(
            ioc_,
            [this]() -> net::awaitable<void>
            {
                // CONFIG SET for keyspace notifications
                {
                    bredis::request req;
                    req.push("CONFIG", "SET", "notify-keyspace-events", "KEA");
                    boost::system::error_code ec;
                    co_await conn_.async_exec(
                        req,
                        bredis::ignore,
                        net::redirect_error(net::deferred, ec));
                    if (ec)
                    {
                        std::println(stderr,
                                     "warning: Could not set "
                                     "notify-keyspace-events: {}",
                                     ec.message());
                    }
                }

                // PSUBSCRIBE
                {
                    bredis::request req;
                    req.push("PSUBSCRIBE",
                             std::string(keyspace_all_devices),
                             std::format("{}*", keyspace_log_names));
                    co_await conn_.async_exec(
                        req, bredis::ignore, net::deferred);
                }

                co_await subscriber_loop();
            },
            net::detached);

        net::co_spawn(
            ioc_,
            [this]() -> net::awaitable<void>
            { co_await conn_.async_run(redis_cfg_, net::deferred); },
            net::detached);

        ioc_thread_ = std::jthread([this](std::stop_token) { ioc_.run(); });
    }

    uint64_t get_devices_version() { return devices_version_.load(); }

    uint64_t get_streams_version(const std::string& device)
    {
        std::scoped_lock lock(mtx_);
        return streams_version_[device];
    }

    bool wait_for_any_change(uint64_t                  old_version,
                             std::chrono::milliseconds timeout)
    {
        std::unique_lock lock(mtx_);
        return cv_any_.wait_for(lock,
                                timeout,
                                [this, old_version]
                                { return any_version_.load() > old_version; });
    }

    uint64_t get_any_version() { return any_version_.load(); }
};

// =============================================================================
// Configuration (identical logic to main.cxx)
// =============================================================================

struct server_config
{
    std::string redis_url            = "tcp://127.0.0.1:6379";
    std::string server_host          = "0.0.0.0";
    int         server_port          = 8080;
    std::string public_dir           = "./08-web/03-redis-web/public";
    int         socket_flags         = 0;
    size_t      max_threads          = 256;
    size_t      log_batch_size       = 1000;
    size_t      redis_pool_size      = 128;
    size_t      sse_poll_interval_ms = 5000;
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
            {
                config.redis_url = j["redis_url"];
            }
            if (j.contains("server_host"))
            {
                config.server_host = j["server_host"];
            }
            if (j.contains("server_port"))
            {
                config.server_port = j["server_port"];
            }
            if (j.contains("public_dir"))
            {
                config.public_dir = j["public_dir"];
            }
            if (j.contains("socket_flags"))
            {
                config.socket_flags = j["socket_flags"];
            }
            if (j.contains("max_threads"))
            {
                config.max_threads = j["max_threads"];
            }
            if (j.contains("log_batch_size"))
            {
                config.log_batch_size = j["log_batch_size"];
            }
            if (j.contains("redis_pool_size"))
            {
                config.redis_pool_size = j["redis_pool_size"];
            }
            if (j.contains("sse_poll_interval_ms"))
            {
                config.sse_poll_interval_ms = j["sse_poll_interval_ms"];
            }
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

// =============================================================================
// HTTP helpers
// =============================================================================

std::string url_decode(std::string_view str)
{
    auto hex_val = [](char c) -> int
    {
        if (c >= '0' && c <= '9')
        {
            return c - '0';
        }
        if (c >= 'a' && c <= 'f')
        {
            return c - 'a' + 10;
        }
        if (c >= 'A' && c <= 'F')
        {
            return c - 'A' + 10;
        }
        return 0;
    };

    std::string result;
    result.reserve(str.size());
    for (std::size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '%' && i + 2 < str.size())
        {
            result += static_cast<char>(hex_val(str[i + 1]) * 16 +
                                        hex_val(str[i + 2]));
            i += 2;
        }
        else if (str[i] == '+')
        {
            result += ' ';
        }
        else
        {
            result += str[i];
        }
    }
    return result;
}

std::string_view mime_type(std::string_view path)
{
    auto dot = path.rfind('.');
    if (dot == std::string_view::npos)
    {
        return "application/octet-stream";
    }
    auto ext = path.substr(dot);
    if (ext == ".html" || ext == ".htm")
    {
        return "text/html";
    }
    if (ext == ".css")
    {
        return "text/css";
    }
    if (ext == ".js")
    {
        return "application/javascript";
    }
    if (ext == ".json")
    {
        return "application/json";
    }
    if (ext == ".png")
    {
        return "image/png";
    }
    if (ext == ".jpg" || ext == ".jpeg")
    {
        return "image/jpeg";
    }
    if (ext == ".gif")
    {
        return "image/gif";
    }
    if (ext == ".svg")
    {
        return "image/svg+xml";
    }
    if (ext == ".ico")
    {
        return "image/x-icon";
    }
    if (ext == ".txt")
    {
        return "text/plain";
    }
    return "application/octet-stream";
}

std::pair<std::string, std::string> split_target(std::string_view target)
{
    auto qpos = target.find('?');
    if (qpos == std::string_view::npos)
    {
        return { std::string(target), {} };
    }
    return { std::string(target.substr(0, qpos)),
             std::string(target.substr(qpos + 1)) };
}

std::unordered_map<std::string, std::string> parse_query(std::string_view query)
{
    std::unordered_map<std::string, std::string> params;
    while (!query.empty())
    {
        auto amp  = query.find('&');
        auto pair = query.substr(0, amp);
        query     = (amp == std::string_view::npos) ? std::string_view{}
                                                    : query.substr(amp + 1);

        auto eq = pair.find('=');
        if (eq != std::string_view::npos)
        {
            params[std::string(pair.substr(0, eq))] =
                url_decode(pair.substr(eq + 1));
        }
    }
    return params;
}

bool write_chunk(tcp::socket& socket, std::string_view data)
{
    auto header = std::format("{:x}\r\n", data.size());
    std::array<net::const_buffer, 3> bufs = { net::buffer(header),
                                              net::buffer(data.data(),
                                                          data.size()),
                                              net::buffer("\r\n", 2) };
    boost::system::error_code        ec;
    net::write(socket, bufs, ec);
    return !ec;
}

void write_last_chunk(tcp::socket& socket)
{
    boost::system::error_code ec;
    net::write(socket, net::buffer(std::string_view("0\r\n\r\n")), ec);
}

net::awaitable<bool> async_write_chunk(tcp::socket&     socket,
                                       std::string_view data)
{
    auto header = std::format("{:x}\r\n", data.size());
    std::array<net::const_buffer, 3> bufs = { net::buffer(header),
                                              net::buffer(data.data(),
                                                          data.size()),
                                              net::buffer("\r\n", 2) };
    boost::system::error_code        ec;
    co_await net::async_write(
        socket, bufs, net::redirect_error(net::deferred, ec));
    co_return !ec;
}

net::awaitable<void> async_write_last_chunk(tcp::socket& socket)
{
    boost::system::error_code ec;
    co_await net::async_write(socket,
                              net::buffer(std::string_view("0\r\n\r\n")),
                              net::redirect_error(net::deferred, ec));
}

// =============================================================================
// Async Redis command helpers (through the shared connection)
// =============================================================================

struct redis_context
{
    bredis::connection& conn;
};

net::awaitable<std::unordered_map<std::string, std::string>> async_hgetall(
    redis_context& ctx, std::string key)
{
    bredis::request req;
    req.push("HGETALL", key);
    bredis::generic_response resp;
    co_await ctx.conn.async_exec(req, resp, net::deferred);
    co_return parse_hgetall(resp);
}

net::awaitable<std::vector<std::string>> async_smembers(redis_context& ctx,
                                                        std::string    key)
{
    bredis::request req;
    req.push("SMEMBERS", key);
    bredis::generic_response resp;
    co_await ctx.conn.async_exec(req, resp, net::deferred);
    co_return parse_smembers(resp);
}

net::awaitable<std::vector<stream_item>> async_xrange(redis_context& ctx,
                                                      std::string    key,
                                                      std::string    start,
                                                      std::string    end,
                                                      std::size_t    count)
{
    bredis::request req;
    req.push("XRANGE", key, start, end, "COUNT", std::to_string(count));
    bredis::generic_response resp;
    co_await ctx.conn.async_exec(req, resp, net::deferred);
    co_return parse_xrange(resp);
}

net::awaitable<std::vector<std::pair<std::string, std::vector<stream_item>>>>
async_xread(redis_context& ctx,
            std::string    key,
            std::string    id,
            std::size_t    count = 1000)
{
    bredis::request req;
    req.push("XREAD", "COUNT", std::to_string(count), "STREAMS", key, id);
    bredis::generic_response resp;

    boost::system::error_code ec;
    co_await ctx.conn.async_exec(
        req, resp, net::redirect_error(net::deferred, ec));
    if (ec)
    {
        co_return std::vector<
            std::pair<std::string, std::vector<stream_item>>>{};
    }
    co_return parse_xread(resp);
}

// =============================================================================
// SSE handler — unified stream (devices + log_names + logs)
// =============================================================================

net::awaitable<void> serve_sse(
    tcp::socket                                         socket,
    const http::request<http::string_body>&             req,
    const std::unordered_map<std::string, std::string>& params,
    redis_context&                                      rctx,
    const std::shared_ptr<notification_center>&         notifier,
    const server_config&                                config)
{
    std::string device;
    std::string stream_key;
    std::string last_event_id;

    if (auto it = params.find("device"); it != params.end())
    {
        device = it->second;
        if (auto it2 = params.find("log_name"); it2 != params.end())
        {
            stream_key = std::format("log:{}:{}", device, it2->second);
        }
    }

    if (auto it = req.find("Last-Event-Id"); it != req.end())
    {
        last_event_id = std::string(it->value());
    }

    http::response<http::empty_body> res{ http::status::ok, req.version() };
    res.set(http::field::content_type, "text/event-stream");
    res.set(http::field::cache_control, "no-cache");
    res.keep_alive(true);
    res.chunked(true);

    http::response_serializer<http::empty_body> sr{ res };
    co_await http::async_write_header(socket, sr, net::deferred);

    auto poll_interval = std::chrono::milliseconds(config.sse_poll_interval_ms);

    // --- Helper lambdas ---

    auto get_devices_json = [&rctx]() -> net::awaitable<json>
    {
        auto devices = co_await async_hgetall(rctx, "all_devices");
        json j       = json::array();
        for (const auto& [name, platform] : devices)
        {
            j.push_back({ { "name", name }, { "platform", platform } });
        }
        co_return j;
    };

    auto get_log_names_json = [&rctx, &device]() -> net::awaitable<json>
    {
        auto names =
            co_await async_smembers(rctx, std::format("log_names:{}", device));
        co_return json(names);
    };

    auto items_to_json = [](const auto& items) -> json
    {
        auto to_entry = [](const stream_item& si) -> json
        {
            const auto& [id, fields] = si;
            json entry;
            entry["id"] = id;
            for (const auto& [key, value] : fields)
            {
                entry[key] = value;
            }
            return entry;
        };
        return json(items | std::views::transform(to_entry) |
                    std::ranges::to<std::vector<json>>());
    };

    auto send_sse = [&socket](std::string_view event,
                              const json&      data) -> net::awaitable<bool>
    {
        auto msg = std::format("event: {}\ndata: {}\n\n", event, data.dump());
        co_return co_await async_write_chunk(socket, msg);
    };

    auto send_sse_with_id =
        [&socket](std::string_view event,
                  const json&      data,
                  std::string_view id) -> net::awaitable<bool>
    {
        auto msg = std::format(
            "id: {}\nevent: {}\ndata: {}\n\n", id, event, data.dump());
        co_return co_await async_write_chunk(socket, msg);
    };

    // --- Streaming logic ---

    try
    {
        auto devices_ver = notifier->get_devices_version();
        if (!co_await send_sse("devices_init", co_await get_devices_json()))
        {
            co_return;
        }

        uint64_t log_names_ver = 0;
        if (!device.empty())
        {
            log_names_ver = notifier->get_streams_version(device);
            if (!co_await send_sse("log_names_init",
                                   co_await get_log_names_json()))
            {
                co_return;
            }
        }

        std::string last_log_id = last_event_id.empty() ? "0-0" : last_event_id;

        if (!stream_key.empty() && last_event_id.empty())
        {
            bool first_batch = true;
            while (true)
            {
                auto start = (last_log_id == "0-0")
                                 ? std::string("-")
                                 : std::format("({}", last_log_id);

                auto batch = co_await async_xrange(
                    rctx, stream_key, start, "+", config.log_batch_size);

                if (batch.empty())
                {
                    if (first_batch &&
                        !co_await send_sse("logs_init", json::array()))
                    {
                        co_return;
                    }
                    break;
                }

                auto event  = first_batch ? "logs_init" : "logs_new";
                last_log_id = batch.back().first;
                if (!co_await send_sse_with_id(
                        event, items_to_json(batch), last_log_id))
                {
                    co_return;
                }
                first_batch = false;

                if (batch.size() < config.log_batch_size)
                {
                    break;
                }
            }
        }

        // --- Polling loop ---

        auto              any_ver = notifier->get_any_version();
        net::steady_timer timer(co_await net::this_coro::executor);

        while (true)
        {
            bool any_sent = false;

            if (!stream_key.empty())
            {
                auto new_data =
                    co_await async_xread(rctx, stream_key, last_log_id);

                if (!new_data.empty() && !new_data[0].second.empty())
                {
                    const auto& items = new_data[0].second;
                    last_log_id       = items.back().first;
                    if (!co_await send_sse_with_id(
                            "logs_new", items_to_json(items), last_log_id))
                    {
                        co_return;
                    }
                    any_sent = true;
                }
            }

            // Check device updates
            auto new_devices_ver = notifier->get_devices_version();
            if (new_devices_ver != devices_ver)
            {
                devices_ver = new_devices_ver;
                if (!co_await send_sse("devices_update",
                                       co_await get_devices_json()))
                {
                    co_return;
                }
                any_sent = true;
            }

            if (!device.empty())
            {
                auto new_ver = notifier->get_streams_version(device);
                if (new_ver != log_names_ver)
                {
                    log_names_ver = new_ver;
                    if (!co_await send_sse("log_names_update",
                                           co_await get_log_names_json()))
                    {
                        co_return;
                    }
                    any_sent = true;
                }
            }

            if (!any_sent)
            {
                if (!co_await async_write_chunk(socket, ":\n\n"))
                {
                    co_return;
                }
            }

            // Sleep for poll_interval
            timer.expires_after(poll_interval);
            boost::system::error_code timer_ec;
            co_await timer.async_wait(
                net::redirect_error(net::deferred, timer_ec));
        }
    }
    catch (const boost::system::system_error&)
    {
    }
    catch (const std::exception& e)
    {
        auto msg = std::format("event: error\ndata: {}\n\n", e.what());
        write_chunk(socket, msg);
    }
}

// =============================================================================
// Download handler — chunked plain-text dump
// =============================================================================

net::awaitable<void> serve_download(
    tcp::socket                                         socket,
    const http::request<http::string_body>&             req,
    const std::unordered_map<std::string, std::string>& params,
    redis_context&                                      rctx)
{
    auto device_it   = params.find("device");
    auto log_name_it = params.find("log_name");

    if (device_it == params.end() || log_name_it == params.end())
    {
        http::response<http::string_body> err{ http::status::bad_request,
                                               req.version() };
        err.set(http::field::content_type, "text/plain");
        err.body() = "Missing 'device' or 'log_name' parameter";
        err.prepare_payload();
        co_await http::async_write(socket, err, net::deferred);
        co_return;
    }

    auto stream_key =
        std::format("log:{}:{}", device_it->second, log_name_it->second);

    http::response<http::empty_body> res{ http::status::ok, req.version() };
    res.set(http::field::content_type, "text/plain");
    res.set(http::field::content_disposition,
            std::format("attachment; filename=\"{}\"", log_name_it->second));
    res.chunked(true);

    http::response_serializer<http::empty_body> sr{ res };
    co_await http::async_write_header(socket, sr, net::deferred);

    constexpr std::size_t batch_size = 5000;
    std::string           last_id    = "-";

    while (true)
    {
        auto start_id = (last_id == "-") ? std::string("-") : "(" + last_id;
        auto batch =
            co_await async_xrange(rctx, stream_key, start_id, "+", batch_size);

        if (batch.empty())
        {
            break;
        }

        std::string chunk;
        chunk.reserve(batch.size() * 100);

        for (const auto& [id, fields] : batch)
        {
            for (const auto& [key, value] : fields)
            {
                if (key == "message")
                {
                    chunk += value;
                    break;
                }
            }
            last_id = id;
        }

        if (!chunk.empty() && !co_await async_write_chunk(socket, chunk))
        {
            co_return;
        }

        if (batch.size() < batch_size)
        {
            break;
        }
    }

    co_await async_write_last_chunk(socket);
}

// =============================================================================
// Static-file handler
// =============================================================================

net::awaitable<void> serve_static_file(
    tcp::socket                             socket,
    const http::request<http::string_body>& req,
    const std::string&                      public_dir,
    std::string                             url_path)
{
    if (url_path == "/")
    {
        url_path = "/index.html";
    }

    if (url_path.find("..") != std::string::npos)
    {
        http::response<http::string_body> res{ http::status::forbidden,
                                               req.version() };
        res.set(http::field::content_type, "text/plain");
        res.body() = "Forbidden";
        res.prepare_payload();
        co_await http::async_write(socket, res, net::deferred);
        co_return;
    }

    auto              full_path = public_dir + url_path;
    beast::error_code ec;

    http::response<http::file_body> res{ http::status::ok, req.version() };
    res.body().open(full_path.c_str(), beast::file_mode::scan, ec);

    if (ec)
    {
        http::response<http::string_body> not_found{ http::status::not_found,
                                                     req.version() };
        not_found.set(http::field::content_type, "text/plain");
        not_found.body() = "Not Found";
        not_found.prepare_payload();
        co_await http::async_write(socket, not_found, net::deferred);
        co_return;
    }

    res.set(http::field::content_type, mime_type(url_path));
    res.content_length(res.body().size());
    co_await http::async_write(socket, res, net::deferred);
}

// =============================================================================
// Per-connection session router (coroutine)
// =============================================================================

net::awaitable<void> handle_session(
    tcp::socket                          socket,
    redis_context&                       rctx,
    std::shared_ptr<notification_center> notifier,
    server_config                        config)
{
    try
    {
        beast::flat_buffer               buffer;
        http::request<http::string_body> req;
        co_await http::async_read(socket, buffer, req, net::deferred);

        auto [path, query_string] = split_target(req.target());
        auto params               = parse_query(query_string);

        if (path == "/api/stream")
        {
            co_await serve_sse(
                std::move(socket), req, params, rctx, notifier, config);
        }
        else if (path == "/api/logs/download")
        {
            co_await serve_download(std::move(socket), req, params, rctx);
        }
        else
        {
            co_await serve_static_file(
                std::move(socket), req, config.public_dir, path);
        }
    }
    catch (const boost::system::system_error& e)
    {
        if (e.code() != net::error::eof &&
            e.code() != net::error::connection_reset)
        {
            std::println(stderr, "Session error: {}", e.what());
        }
    }
    catch (const std::exception& e)
    {
        std::println(stderr, "Session error: {}", e.what());
    }

    boost::system::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

} // namespace om

// =============================================================================

int main(int argc, char* argv[])
{
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

    auto url = om::parse_redis_url(config.redis_url);

    // --- Start notification center ---

    auto notifier = std::make_shared<om::notification_center>(url);
    notifier->start();

    // --- Main io_context for HTTP server + shared Redis connection ---

    om::net::io_context    ioc;
    om::bredis::connection redis_conn(ioc);
    om::bredis::config     redis_cfg;
    redis_cfg.addr                    = { url.host, url.port };
    redis_cfg.username                = url.username;
    redis_cfg.password                = url.password;
    redis_cfg.reconnect_wait_interval = std::chrono::seconds(1);
    redis_cfg.health_check_interval   = std::chrono::seconds(5);

    om::redis_context rctx{ redis_conn };

    om::net::co_spawn(
        ioc,
        [&]() -> om::net::awaitable<void>
        { co_await redis_conn.async_run(redis_cfg, om::net::deferred); },
        om::net::detached);

    // --- TCP acceptor ---

    auto              address = om::net::ip::make_address(config.server_host);
    om::tcp::endpoint endpoint(address,
                               static_cast<unsigned short>(config.server_port));

    om::tcp::acceptor acceptor(ioc);
    acceptor.open(endpoint.protocol());
    acceptor.set_option(om::net::socket_base::reuse_address(true));

#if defined(SO_REUSEPORT) && !defined(_WIN32)
    {
        int no = 0;
        setsockopt(acceptor.native_handle(),
                   SOL_SOCKET,
                   SO_REUSEPORT,
                   reinterpret_cast<const void*>(&no),
                   sizeof(no));
    }
#endif

    boost::system::error_code bind_ec;
    acceptor.bind(endpoint, bind_ec);
    if (bind_ec)
    {
        std::println(stderr,
                     "Failed to bind to {}:{}: {}",
                     config.server_host,
                     config.server_port,
                     bind_ec.message());
        return 1;
    }
    acceptor.listen(om::net::socket_base::max_listen_connections);

    std::println(
        "Starting Boost.Beast + Boost.Redis web server on http://{}:{}",
        config.server_host,
        config.server_port);
    std::println("Serving static files from {}", config.public_dir);
    std::println("Log batch size: {}", config.log_batch_size);
    std::println("SSE poll interval: {}ms", config.sse_poll_interval_ms);

    // Accept loop (coroutine)
    om::net::co_spawn(
        ioc,
        [&]() -> om::net::awaitable<void>
        {
            while (true)
            {
                auto socket = co_await acceptor.async_accept(om::net::deferred);
                om::net::co_spawn(
                    ioc,
                    om::handle_session(
                        std::move(socket), rctx, notifier, config),
                    om::net::detached);
            }
        },
        om::net::detached);

    ioc.run();

    return 0;
}
