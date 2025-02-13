#include <functional>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <fcgiapp.h>

namespace om
{
struct web_app_context
{
    std::string socket_path = "127.0.0.1:9000";
    int         queue_size  = 20;
    // хранит дескриптор открытого сокета
    int        socket_id{};
    std::mutex accept_mutex;
};

void worker_job(web_app_context& context)
{
    using namespace std::string_literals;

    int          rc;
    FCGX_Request request;
    // std::string  server_name;

    if (FCGX_InitRequest(&request, context.socket_id, 0) != 0)
    {
        // ошибка при инициализации структуры запроса
        std::cerr << "Can not init request\n";
        return;
    }
    std::cout << "Request is inited\n";

    auto accept_request = [](std::mutex&   accept_mutex,
                             FCGX_Request& request) -> bool
    {
        int rc = FCGX_Accept_r(&request);
        return rc >= 0;
    };
    // попробовать получить новый запрос
    std::cout << "Try to accept new request" << std::endl;

    while (accept_request(context.accept_mutex, request))
    {
        std::cout << "request is accepted" << std::endl;

        std::stringstream out;

        out << "Content-type: text/html\r\n"
               "\r\n"
               "<html>\r\n"
               "    <head>\r\n"
               "        <title>FastCGI Hello from C++!</title>\r\n"
               "    </head>\r\n"
               "    <body>\r\n"
               "        <h1>FastCGI Hello! C++ fcgiapp</h1>\r\n"
               "        <p>All Request params: <br/>\r\n";

        for (char** current = request.envp; *current; current++)
        {
            // clang-format off
        out << "            <i>" << *current << "</i><br/>\r\n";
            // clang-format on
        }

        out << "        </p>\r\n"
               "    </body>\r\n"
               "</html>\r\n";
        auto str = out.str();
        FCGX_PutStr(str.data(), static_cast<int>(str.size()), request.out);

        // закрыть текущее соединение
        FCGX_Finish_r(&request);
    }
    std::cout << "fishish main loop" << std::endl;
}

} // namespace om

int main(int argc, char** argv)
{
    om::web_app_context context;
    size_t              num_of_cpu = 1; // std::thread::hardware_concurrency();
    std::vector<std::jthread> threads;
    threads.reserve(num_of_cpu);

    // инициализация библилиотеки
    if (0 != FCGX_Init())
    {
        std::cerr << "error: failed to FCGX_init()" << std::endl;
        return 1;
    }
    std::cout << "FCGX_init is inited" << std::endl;

    // открываем новый сокет
    context.socket_id =
        FCGX_OpenSocket(context.socket_path.c_str(), context.queue_size);
    if (context.socket_id < 0)
    {
        // ошибка при открытии сокета
        std::cerr << "error: failed to OpenSocket: " << context.socket_path
                  << " error: " << context.socket_id << std::endl;
        return 1;
    }
    std::cout << "Socket is opened" << std::endl;

    // создаём рабочие потоки
    while (num_of_cpu--)
    {
        threads.emplace_back(om::worker_job, std::ref(context));
    }

    return 0;
}
