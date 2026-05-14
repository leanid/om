#include <future>
#include <iostream>
#include <map>
#include <thread>
#include <vector>

namespace om
{
struct data
{
    std::string                         str;
    std::vector<std::int32_t>           values;
    std::map<std::int32_t, std::string> key_value;
};
} // namespace om

int main()
{
    // future from a packaged_task
    std::packaged_task<om::data(std::stop_token)> task(
        [](std::stop_token stoken) -> om::data
        {
            if (stoken.stop_requested())
            {
                return {};
            }
            std::srand(std::time(nullptr));
            if (std::rand() % 2)
            {
                throw std::runtime_error("exceptions can be used too");
            }

            // we can return any complex struct from job thread
            return { .str       = "string",
                     .values    = { 1, 2, 3, 4, 5 },
                     .key_value = { { 0, "zero" }, { 1, "one" } } };
        }); // wrap the function

    std::future<om::data> future =
        task.get_future(); // get a future before pass task to thread (before
                           // std::move)
    {
        // scoped thread to do the task
        std::jthread t(std::move(task)); // launch on a thread

        // comment out next line to return filled structure
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        t.request_stop();
    }

    om::data data;

    try
    {
        using namespace std::chrono_literals;
        // check status without blocking
        auto status = future.wait_for(0s);

        if (status == std::future_status::ready)
        {
            std::cout << "task is ready!\n";
            std::cout << "Exceptions are rethrown upon calling .get()!\n";
        }

        data = future.get();
    }
    catch (std::runtime_error& ex)
    {
        std::cout << "we got exception during task calculation on jthread: "
                  << ex.what() << std::endl;
    }

    std::cout << data.str << std::endl;
    for (auto value : data.values)
    {
        std::cout << value << ",";
    }
    std::cout << std::endl;

    return 0;
}
