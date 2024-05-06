#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

extern "C"
{
#include <fcgiapp.h>
}

namespace om
{
constexpr const char* socket_path = "127.0.0.1:9000";
// хранит дескриптор открытого сокета
static int socket_id;
} // namespace om

static void worker_job(std::mutex& accept_mutex)
{
    using namespace std::string_literals;

    int          rc;
    FCGX_Request request;
    std::string  server_name;

    if (FCGX_InitRequest(&request, om::socket_id, 0) != 0)
    {
        // ошибка при инициализации структуры запроса
        printf("Can not init request\n");
        return;
    }
    std::cout << "Request is inited\n";

    for (;;)
    {
        // попробовать получить новый запрос
        std::cout << "Try to accept new request\n";
        {
            std::lock_guard lock(accept_mutex);
            rc = FCGX_Accept_r(&request);
        }

        if (rc < 0)
        {
            // ошибка при получении запроса
            std::cerr << "Can not accept new request\n";
            break;
        }
        std::cout << "request is accepted\n";

        // получить значение переменной
        server_name = FCGX_GetParam("SERVER_NAME", request.envp);

        // вывести все HTTP-заголовки (каждый заголовок с новой строки)
        FCGX_PutS("Content-type: text/html\r\n", request.out);
        // между заголовками и телом ответа нужно вывести пустую строку
        FCGX_PutS("\r\n", request.out);
        // вывести тело ответа (например - html-код веб-страницы)
        FCGX_PutS("<html>\r\n", request.out);
        FCGX_PutS("<head>\r\n", request.out);
        FCGX_PutS("<title>FastCGI Hello! (multi-threaded C, fcgiapp "
                  "library)</title>\r\n",
                  request.out);
        FCGX_PutS("</head>\r\n", request.out);
        FCGX_PutS("<body>\r\n", request.out);
        FCGX_PutS(
            "<h1>FastCGI Hello! (multi-threaded C, fcgiapp library)</h1>\r\n",
            request.out);
        FCGX_PutS("<p>Request accepted from host <i>", request.out);
        FCGX_PutS(server_name.empty() ? "?" : server_name.data(), request.out);
        FCGX_PutS("</i></p>\r\n", request.out);
        FCGX_PutS("</body>\r\n", request.out);
        FCGX_PutS("</html>\r\n", request.out);

        //"заснуть" - имитация многопоточной среды
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2000ms);

        // закрыть текущее соединение
        FCGX_Finish_r(&request);

        // завершающие действия - запись статистики, логгирование ошибок и т.п.
    }
}

int main(int argc, char** argv)
{
    std::mutex                accept_mutex;
    size_t                    num_of_cpu = std::thread::hardware_concurrency();
    std::vector<std::jthread> threads;
    threads.reserve(num_of_cpu);

    // инициализация библилиотеки
    FCGX_Init();
    std::cout << "Lib is inited\n";

    // открываем новый сокет
    om::socket_id = FCGX_OpenSocket(om::socket_path, 20); // 20 - queue size
    if (om::socket_id < 0)
    {
        // ошибка при открытии сокета
        std::cerr << "error: failed to OpenSocket: " << om::socket_path
                  << " error: " << om::socket_id << std::endl;
        return 1;
    }
    std::cout << "Socket is opened\n";

    // создаём рабочие потоки
    while (num_of_cpu--)
    {
        threads.push_back(std::jthread(worker_job, std::ref(accept_mutex)));
    }

    return 0;
}
