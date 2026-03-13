#include <sw/redis++/redis++.h>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace om
{

namespace redis = sw::redis;

constexpr auto key_ttl = std::chrono::hours(72); // 3 days
// Функция для генерации имени файла лога на основе текущего времени
std::string generate_log_file_name()
{
    auto        now    = std::chrono::system_clock::now();
    std::time_t now_c  = std::chrono::system_clock::to_time_t(now);
    std::tm*    now_tm = std::localtime(&now_c);

    std::ostringstream oss;
    oss << "log_" << std::put_time(now_tm, "%d_%m_%Y") << "T"
        << std::put_time(now_tm, "%H_%M_%S") << ".txt";
    return oss.str();
}

void log_to_redis(redis::Redis&      redis_client,
                  const std::string& stream_key,
                  const std::string& level,
                  const std::string& message)
{
    // Получаем текущее время
    auto        now    = std::chrono::system_clock::now();
    std::time_t now_c  = std::chrono::system_clock::to_time_t(now);
    std::tm*    now_tm = std::localtime(&now_c);
    auto        ms     = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    // Формируем единую строку лога: [HH:MM:SS.ms] LEVEL Message
    std::ostringstream oss;
    oss << "[" << std::put_time(now_tm, "%H:%M:%S") << "." << std::setfill('0')
        << std::setw(3) << ms.count() << "] " << level << " " << message;

    std::string formatted_message = oss.str();

    try
    {
        // Отправляем только одно поле - message
        redis_client.xadd(
            stream_key, "*", { std::make_pair("message", formatted_message) });
        std::cout << formatted_message << std::endl;
    }
    catch (const redis::Error& e)
    {
        std::cerr << "Redis XADD error: " << e.what() << std::endl;
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
