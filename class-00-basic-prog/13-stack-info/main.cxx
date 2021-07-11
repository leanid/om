#include <iostream>
#include <memory>
#include <numeric>

#include "stack_info.hxx"

constexpr size_t stack_frame_size = 512 * 10;

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
                                size_t          any_value)
{
    std::array<char, stack_frame_size> tmp_one_kb;

    // read memory - we don't let OS any chance not to allocation it
    char any_value_local =
        std::accumulate(begin(tmp_one_kb), end(tmp_one_kb), char()) + any_value;

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

    size_t page_size = 4096;

    size_t validation_limit = frame_size > page_size ? frame_size : page_size;

    if (current_stack_info.get_current_stack_position() + validation_limit >=
        current_stack_info.get_stack_size())
    {
        std::cout << "frame_size: " << frame_size << '\n'
                  << "page_size: " << page_size << '\n'
                  << "validation_limit: " << validation_limit << std::endl;
        return frame_index;
    }

    return recursive_stack_checking(tmp_one_kb,
                                    frame_index + 1,
                                    dump_every_nth_frame,
                                    current_stack_info,
                                    any_value_local);
}

int main(int argc, char**)
{
    om::stack_info stack_info;

    std::cout << stack_info << " "
              << (stack_info.get_stack_size() / (1024 * 1024)) << "MB"
              << std::endl;

    char  a;
    char* a_ptr = &a;
    *a_ptr      = 'A';

    om::stack_info stack_info_next;

    if (stack_info.get_stack_size() != stack_info_next.get_stack_size())
    {
        std::cout << "Why!!! On same stack frame different stack size?\n";
    }

    if (argc > 2) // experiment - write below stack
    {
        // lets write some byte below current stack! And check stack again!
        char* ptr = reinterpret_cast<char*>(stack_info.get_address_min());
        ptr -= 1024; // lets move 1kb below

        *ptr = 'A';

        // now lets check again stack size
        om::stack_info stack_info_after_write_below;

        std::cout << stack_info_after_write_below << std::endl;

        if (stack_info.get_stack_size() !=
            stack_info_after_write_below.get_stack_size())
        {
            std::cout
                << "as you can see on Linux OS system dynamically increase "
                   "stack segment for current thread\n";
        }
    }

    if (argc > 1)
    {
        std::array<char, stack_frame_size> one_frame_size;
        size_t                             max_stack_frame =
            recursive_stack_checking(one_frame_size, 1, 100, stack_info, 0);
        std::cout << "max stack frame: " << max_stack_frame << std::endl;
    }

    return std::cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
}
