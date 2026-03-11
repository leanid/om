#include <sw/redis++/redis++.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace sw::redis;

void log_to_redis(Redis&             redis,
                  const std::string& stream_name,
                  const std::string& level,
                  const std::string& message)
{
    // Получаем текущий timestamp в миллисекундах
    auto now = std::chrono::system_clock::now();
    auto timestamp =
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                           now.time_since_epoch())
                           .count());

    try
    {
        // Отправляем данные в stream (XADD stream_name * timestamp <ts> level
        // <level> message <message>)
        // "*" означает, что Redis сам сгенерирует ID для записи в stream
        redis.xadd(stream_name,
                   "*",
                   { std::make_pair("timestamp", timestamp),
                     std::make_pair("level", level),
                     std::make_pair("message", message) });
        std::cout << "Logged [" << level << "]: " << message << std::endl;
    }
    catch (const Error& e)
    {
        std::cerr << "Redis XADD error: " << e.what() << std::endl;
    }
}

int main()
{
    // Читаем строку подключения из файла
    std::ifstream config_file(
        "./00-basic-prog/41-redis-log-client/redis_config.txt");
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
        Redis redis(connection_string);

        // Имя ключа стрима
        std::string stream_name = "leo_phone_11_03_2026";

        std::cout << "Connecting to Redis at " << connection_string << "..."
                  << std::endl;

        // Отправляем несколько заготовленных логов
        log_to_redis(
            redis, stream_name, "INFO", "Application started successfully.");
        log_to_redis(
            redis, stream_name, "DEBUG", "Initializing internal modules...");
        log_to_redis(redis,
                     stream_name,
                     "WARNING",
                     "Config file not found, using default parameters.");
        log_to_redis(redis,
                     stream_name,
                     "ERROR",
                     "Failed to connect to secondary database!");
        log_to_redis(redis, stream_name, "INFO", "Application shutting down.");

        std::cout << "All logs sent to stream '" << stream_name << "'."
                  << std::endl;
    }
    catch (const Error& e)
    {
        std::cerr << "Redis connection/execution error: " << e.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
