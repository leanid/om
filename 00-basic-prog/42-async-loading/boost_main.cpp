#define BOOST_THREAD_PROVIDES_FUTURE

#include <boost/chrono/duration.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/thread/future.hpp>
#include <boost/thread/thread.hpp>

#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
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
    auto promise = std::make_shared<boost::promise<om::data>>();
    boost::future<om::data> future = promise->get_future();

    boost::thread worker(
        [promise]()
        {
            try
            {
                boost::this_thread::interruption_point();

                std::srand(static_cast<unsigned int>(std::time(nullptr)));
                if (std::rand() % 2)
                {
                    throw std::runtime_error("exceptions can be used too");
                }

                // we can return any complex struct from job thread
                promise->set_value(
                    { .str       = "string",
                      .values    = { 1, 2, 3, 4, 5 },
                      .key_value = { { 0, "zero" }, { 1, "one" } } });
            }
            catch (const boost::thread_interrupted&)
            {
                promise->set_value({});
            }
            catch (...)
            {
                promise->set_exception(boost::current_exception());
            }
        });

    // comment out next line to return filled structure
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
    worker.interrupt();
    worker.join();

    om::data data;

    try
    {
        // check status without blocking
        auto status = future.wait_for(boost::chrono::seconds(0));

        if (status == boost::future_status::ready)
        {
            std::cout << "task is ready!\n";
            std::cout << "Exceptions are rethrown upon calling .get()!\n";
        }

        data = future.get();
    }
    catch (std::runtime_error& ex)
    {
        std::cout << "we got exception during task calculation on boost::thread: "
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
