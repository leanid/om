#include <array>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <numeric>
#include <sstream>
#include <string_view>
#include <thread>

#include "stack_info.hxx"

constexpr size_t stack_frame_size = 512ull * 10;

std::mutex              m;
std::condition_variable cv;

std::ostream& operator<<(std::ostream& stream, const om::stack_info& stack_info)
{
    using namespace std;

    stream << std::hex << stack_info.get_address_min() << '-' << std::hex
           << stack_info.get_address_max() << std::dec;

    return stream;
}

size_t recursive_stack_checking(std::array<char, stack_frame_size>& prev,
                                size_t                              frame_index,
                                size_t          dump_every_nth_frame,
                                om::stack_info& prev_stack_info,
                                size_t          any_value, // NOLINT
                                size_t          thread_index)
{
    std::array<char, stack_frame_size> tmp_one_kb;

    // read memory - we don't let OS any chance not to allocation it
    char any_value_local = static_cast<char>(
        std::accumulate(begin(tmp_one_kb), end(tmp_one_kb), char()) +
        any_value);

    om::stack_info current_stack_info;

    if (current_stack_info.get_stack_size() != prev_stack_info.get_stack_size())
    {
        std::cout
            << "Warning OS just increased stack segment for current thread!\n"
            << "old: [" << prev_stack_info << "]\n"
            << "new: [" << current_stack_info << "]\n"
            << "current stack frame index: " << frame_index << '\n'
            << std::endl;
    }

    size_t frame_size =
        reinterpret_cast<char*>(&prev) - reinterpret_cast<char*>(&tmp_one_kb);

    size_t guard_size = current_stack_info.get_quard_size() > 0
                            ? current_stack_info.get_quard_size()
                            : 4096;

    size_t validation_limit = frame_size + guard_size;

    if (current_stack_info.get_current_stack_position() + validation_limit >=
        current_stack_info.get_stack_size())
    {
        std::cout << thread_index << " frame_size: " << frame_size << '\n'
                  << thread_index << " guard_size: " << guard_size << '\n'
                  << thread_index << " validation_limit: " << validation_limit
                  << '\n'
                  << thread_index << " frame_index: " << frame_index
                  << std::endl;
        return frame_index;
    }

    return recursive_stack_checking(tmp_one_kb,
                                    frame_index + 1,
                                    dump_every_nth_frame,
                                    current_stack_info,
                                    any_value_local,
                                    thread_index);
}

void print_stack_info(const om::stack_info& info, size_t thread_index)
{
    using namespace std;
    stringstream ss;
    ss << thread_index << " " << info << " " << fixed
       << (static_cast<double>(info.get_stack_size()) / (1024.0 * 1024.0))
       << "MB" << endl;
    cout << ss.str();
}

void thread_stack_check(size_t index)
{
    using namespace std;

    om::stack_info stack_info;

    print_stack_info(stack_info, index);

    array<char, stack_frame_size> one_frame_size;
    size_t                        max_stack_frame =
        recursive_stack_checking(one_frame_size, 1, 100, stack_info, 0, index);

    stringstream ss;
    ss << "thread " << index << " max stack frame: " << max_stack_frame << endl;

    {
        // std::unique_lock lock(m);
        // cv.wait_for(lock, std::chrono::minutes(10));
    }
    cout << ss.str(); // try to do it atomically
}

int main_safe(int argc, const char** argv);

int main(int argc, const char** argv)
{
    try
    {
        main_safe(argc, argv);
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what();
    }
}

int main_safe(int argc, const char** argv)
{
    using namespace std;

    om::stack_info stack_info;

    print_stack_info(stack_info, 0);

    char  a;
    char* a_ptr = &a;
    *a_ptr      = 'A';

    om::stack_info stack_info_next;

    if (stack_info.get_stack_size() != stack_info_next.get_stack_size())
    {
        cout << "Why!!! On same stack frame different stack size?\n";
    }

    if (argv[argc - 1] == "below"sv) // experiment - write below stack
    {
        // lets write some byte below current stack! And check stack again!
        char* ptr = reinterpret_cast<char*>(stack_info.get_address_min());
        ptr -= 1024; // lets move 1kb below

        *ptr = 'A'; // Segmentation fault (core dumped)
                    // term>coredumpctl debug (on Fedora)

        // now lets check again stack size
        om::stack_info stack_info_after_write_below;

        cout << stack_info_after_write_below << endl;

        if (stack_info.get_stack_size() !=
            stack_info_after_write_below.get_stack_size())
        {
            cout << "as you can see on Linux OS system dynamically increase "
                    "stack segment for current thread\n";
        }
    }

    if (argv[argc - 1] == "recursive"sv)
    {
        array<char, stack_frame_size> one_frame_size;
        size_t                        max_stack_frame =
            recursive_stack_checking(one_frame_size, 1, 100, stack_info, 0, 0);
        cout << "max stack frame: " << max_stack_frame << endl;
    }

    if (argv[argc - 1] == "recursive_threads"sv)
    {
        // create 3 more threads to check it's stack size and
        // position in memory process
        array<thread, 3> threads = { thread(thread_stack_check, 1),
                                     thread(thread_stack_check, 2),
                                     thread(thread_stack_check, 3) };

        thread_stack_check(0); // current main thread stack checking

        for (auto& t : threads)
        {
            t.join();
        }
    }

    return cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
}
