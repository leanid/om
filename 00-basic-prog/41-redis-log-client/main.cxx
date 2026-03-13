#include <sw/redis++/redis++.h>

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <vector>

namespace om
{

namespace redis = sw::redis;

constexpr auto key_ttl = std::chrono::hours(72); // 3 days

std::tm to_local_tm(std::chrono::system_clock::time_point tp)
{
    auto    t = std::chrono::system_clock::to_time_t(tp);
    std::tm result{};
#ifdef _WIN32
    localtime_s(&result, &t);
#else
    localtime_r(&t, &result);
#endif
    return result;
}

std::string generate_log_file_name()
{
    auto tm = to_local_tm(std::chrono::system_clock::now());
    return std::format("log_{:02}_{:02}_{}T{:02}_{:02}_{:02}.txt",
                       tm.tm_mday,
                       tm.tm_mon + 1,
                       tm.tm_year + 1900,
                       tm.tm_hour,
                       tm.tm_min,
                       tm.tm_sec);
}

void log_to_redis(redis::Redis&      redis_client,
                  const std::string& stream_key,
                  const std::string& level,
                  const std::string& message)
{
    auto now = std::chrono::system_clock::now();
    auto tm  = to_local_tm(now);
    auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    auto formatted_message = std::format("[{:02}:{:02}:{:02}.{:03}] {} {}",
                                         tm.tm_hour,
                                         tm.tm_min,
                                         tm.tm_sec,
                                         ms.count(),
                                         level,
                                         message);

    try
    {
        redis_client.xadd(
            stream_key, "*", { std::make_pair("message", formatted_message) });
        std::println("{}", formatted_message);
    }
    catch (const redis::Error& e)
    {
        std::println(stderr, "Redis XADD error: {}", e.what());
    }
}

} // namespace om

int main()
{
    namespace redis = sw::redis;
    // Читаем строку подключения из файла
    std::ifstream config_file(
        "./00-basic-prog/41-redis-log-client/log_client_config.txt");
    std::string connection_string;

    if (config_file.is_open())
    {
        std::getline(config_file, connection_string);
    }
    else
    {
        std::cerr << "Failed to open redis_config.txt. Using default "
                     "tcp://127.0.0.1:6379"
                  << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        // Подключаемся к Redis
        redis::Redis redis_client(connection_string);

        // Имя устройства
        std::string device_name = "anastasia_device";

        // Платформа устройства
        std::string platform = "MacOS";

        // Генерируем имя стрима (файла)
        std::string log_file_name = om::generate_log_file_name();

        // Полный ключ для стрима в Redis: log:device_name:stream_name
        std::string stream_key = "log:" + device_name + ":" + log_file_name;

        // Регистрируем устройство и его платформу в общем HASH
        redis_client.hset("all_devices", device_name, platform);
        redis_client.expire("all_devices", om::key_ttl);

        // Регистрируем текущий лог файл для этого устройства
        redis_client.sadd("log_names:" + device_name, log_file_name);
        redis_client.expire("log_names:" + device_name, om::key_ttl);

        std::cout << "Connecting to Redis at " << connection_string << "..."
                  << std::endl;
        std::cout << "Device: " << device_name << std::endl;
        std::cout << "Log: " << log_file_name << std::endl;

        // Отправляем несколько заготовленных логов
        om::log_to_redis(redis_client,
                         stream_key,
                         "INFO",
                         "Application started successfully.");
        om::log_to_redis(redis_client,
                         stream_key,
                         "DEBUG",
                         "Initializing internal modules...");
        om::log_to_redis(redis_client,
                         stream_key,
                         "WARNING",
                         "Config file not found, using default parameters.");
        om::log_to_redis(redis_client,
                         stream_key,
                         "ERROR",
                         "Failed to connect to secondary database!");

        redis_client.expire(stream_key, om::key_ttl);

        std::cout << "Initial logs sent to stream '" << stream_key << "'."
                  << std::endl;

        std::cout << "\n=== Interactive Mode ===" << std::endl;
        std::cout << "Type a message and press Enter to send it as a log."
                  << std::endl;
        std::cout << "Type 'quit' or 'exit' to stop the application.\n"
                  << std::endl;

        std::string user_input;
        while (true)
        {
            std::cout << "Log message > ";
            if (!std::getline(std::cin, user_input))
            {
                break; // Выход по EOF (Ctrl+D)
            }

            if (user_input == "quit" || user_input == "exit")
            {
                break;
            }

            if (!user_input.empty())
            {
                om::log_to_redis(redis_client, stream_key, "INFO", user_input);
            }
        }

        om::log_to_redis(
            redis_client, stream_key, "INFO", "Application shutting down.");
    }
    catch (const redis::Error& e)
    {
        std::cerr << "Redis connection/execution error: " << e.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
