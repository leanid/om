#if defined(_WIN32)
#define _WIN32_WINNT 0x0602
#include <windows.h>

static void get_stack_min_and_max_adresses(size_t& min, size_t& max)
{
    // works if Windows version >= 8
    GetCurrentThreadStackLimits(&min, &max);
}
static size_t get_stack_quard_size()
{
    return 0;
}
#else

#include <algorithm> // c++20 std::shift_left // NOLINT
#include <array>
#include <charconv>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <limits>
#include <string_view>

#include <fcntl.h>
#include <pthread.h>
#include <sys/resource.h>
#include <unistd.h>

bool find_stack_min_and_max_adresses(const std::string_view& line,
                                     size_t&                 min,
                                     size_t&                 max)
{
    size_t index = line.find(std::string_view{ "[stack]", 7 });
    if (std::string_view::npos == index)
    {
        return false;
    }
    // parse example: 7fd4a82000-7fd4aa3000 rw-p 00000000 00:00 0  [stack]
    size_t index_of_dash = line.find('-');
    if (std::string_view::npos == index_of_dash)
    {
        return false;
    }
    size_t index_of_first_space = line.find(' ', index_of_dash);
    if (std::string_view::npos == index_of_first_space)
    {
        return false;
    }
    std::string_view min_addr = line.substr(0, index_of_dash);
    std::string_view max_addr = line.substr(
        index_of_dash + 1, index_of_first_space - index_of_dash - 1);

    std::from_chars_result result = std::from_chars(
        min_addr.data(), min_addr.data() + min_addr.size(), min, 16);
    if (result.ec != std::errc())
    {
        return false;
    }
    result = std::from_chars(
        max_addr.data(), max_addr.data() + max_addr.size(), max, 16);
    if (result.ec != std::errc())
    {
        return false;
    }

    struct rlimit rl;
    int           result_rl = getrlimit(RLIMIT_STACK, &rl);
    if (result_rl == 0)
    {
        min = max - rl.rlim_cur;
    }

    return true;
}

static void get_stack_min_and_max_adresses(size_t& min, size_t& max)
{

    pthread_attr_t attr;
    pthread_t      self = pthread_self();
    int            err  = pthread_getattr_np(self, &attr);
    if (0 == err)
    {
        void*  stack_start_addr;
        size_t stack_size;
        err = pthread_attr_getstack(&attr, &stack_start_addr, &stack_size);

        if (0 == err)
        {
            min = reinterpret_cast<size_t>(stack_start_addr);
            max = min + stack_size;
        }

        err = pthread_attr_destroy(&attr);

        if (err)
        {
            std::cerr << "pthread_attr_destroy failed: " << std::strerror(err);
        }
        return;
    }
    else
    {
        std::cerr << "pthread_getaddr_np failed errno=" << std::strerror(err);
    }

    // fallback implementation
    int fd = open("/proc/self/maps", O_RDONLY);

    if (fd < 0)
    {
        min = max = 0;
        return;
    }

    std::array<char, 1024> line;
    ssize_t                status{ 0 };

    status = read(fd, line.data(), line.size());
    if (status <= 0)
    {
        // end of file
        min = max = 0;
        return;
    }

    while (true)
    {
        std::string_view str(line.data(), static_cast<size_t>(status));
        size_t           new_line_index = str.find('\n');
        if (new_line_index != std::string_view::npos)
        {
            std::string_view next_line(line.data(), new_line_index);
            if (find_stack_min_and_max_adresses(next_line, min, max))
            {
                close(fd);
                return;
            }
            else
            {
// skip current line
#if defined(__cpp_lib_shift) && __cpp_lib_shift >= 202202L
                std::ranges::shift_left(line, new_line_index + 1);
#else
                std::memmove(line.data(),
                             line.data() + new_line_index + 1,
                             status - new_line_index - 1);
#endif
                status -= new_line_index + 1; // NOLINT
            }
        }
        else
        {
            // no more \n in current buffer -> read more from file
            ssize_t next_status =
                read(fd, line.data() + status, line.size() - status);
            status += next_status;
            if (next_status <= 0)
            {
                min = max = 0;
                close(fd);
                return;
            }
        }
    }
}

static size_t get_stack_quard_size()
{
    size_t         guard_size = std::numeric_limits<size_t>::max(); // bad value
    pthread_attr_t attr;
    pthread_t      self = pthread_self();
    int            err  = pthread_getattr_np(self, &attr);
    if (0 == err)
    {
        size_t guard_size_tmp;
        err = pthread_attr_getguardsize(&attr, &guard_size_tmp);

        if (0 == err)
        {
            guard_size = guard_size_tmp;
        }

        err = pthread_attr_destroy(&attr);

        if (err)
        {
            std::cerr << "pthread_attr_destroy failed: " << std::strerror(err);
        }
    }
    return guard_size;
}
#endif

#include "stack_info.hxx"

namespace om
{
stack_info::stack_info()
{
    get_stack_min_and_max_adresses(address_min, address_max);
}

size_t stack_info::get_stack_size() const
{
    return address_max - address_min;
}

size_t stack_info::get_current_stack_position() const
{
    char              c;
    const char* const ptr = &c;
    return address_max - reinterpret_cast<size_t>(ptr);
}

size_t stack_info::get_free_stack_memory_size() const
{
    return get_stack_size() - get_current_stack_position();
}

size_t stack_info::get_quard_size() const
{
    return get_stack_quard_size();
}

} // namespace om
