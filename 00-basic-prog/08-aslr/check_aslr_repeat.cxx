#include "exec-stream.h"

#include <algorithm>
#include <atomic>
#include <future>
#include <iostream>
#include <mutex>
#include <numeric>
#include <optional>
#include <sstream>
#include <thread>
#include <unordered_set>

const std::string_view program_to_run{ "./08-aslr" };

std::unordered_set<std::string> previous_results;
std::mutex                      result_guard;
std::atomic<bool>               found_match_flag{};

static bool                       safe_insert(const std::string& result);
static std::optional<std::string> one_iter();
static size_t                     thread_func();
static void                       run_program_while_find_same_stack_adress();

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    try
    {
        run_program_while_find_same_stack_adress();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static bool safe_insert(const std::string& result)
{
    std::lock_guard<std::mutex> lock(result_guard);
    auto [it, is_succeded] = previous_results.insert(result);
    return is_succeded;
}

static std::optional<std::string> one_iter()
{
    using namespace std;

    exec_stream_t stream(program_to_run.data(), "");

    stringstream output;
    output << stream.out().rdbuf();
    string result = output.str();

    bool is_inserted = safe_insert(result);
    if (is_inserted)
    {
        return std::nullopt;
    }
    return { std::move(result) };
}

static size_t thread_func()
{
    size_t i = 0;
    for (; !found_match_flag && i < 100000; ++i)
    {
        if (auto duplicate_found = one_iter(); duplicate_found)
        {
            found_match_flag = true;
            std::cout << "next stack addresses already exist:\n"
                      << duplicate_found.value() << std::endl;
            break;
        }
    }
    return i;
};

static void run_program_while_find_same_stack_adress()
{
    using namespace std;
    const size_t num_cores = thread::hardware_concurrency();
    cout << "run on " << num_cores << " cores" << endl;

    vector<future<size_t>> jobs(num_cores);

    for_each(begin(jobs),
             end(jobs),
             [](future<size_t>& num_iter) { num_iter = async(thread_func); });

    cout << "all thread are running..." << endl;

    size_t iter_count = accumulate(begin(jobs),
                                   end(jobs),
                                   size_t(0UL),
                                   [](size_t current, future<size_t>& num_iter)
                                   {
                                       static size_t thread_index = 0;
                                       size_t iter_count = num_iter.get();
                                       std::cout << "thread " << thread_index++
                                                 << " complete " << iter_count
                                                 << " iterations" << std::endl;
                                       return current + iter_count;
                                   });

    cout << "we found match in " << iter_count << " iterations\n"
         << "running same program(" << program_to_run << ")" << endl
         << "with same stack adresses" << endl;
}
