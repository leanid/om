#if defined(_WIN32)
#define _WIN32_WINNT 0x0602
#include <processthreadsapi.h>

static void get_stack_min_and_max_adresses(size_t& min, size_t& max)
{
    GetCurrentThreadStackLimits(&min, &max);
}

#else
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
    size_t tmp;
    return address_max - reinterpret_cast<size_t>(&tmp);
}

size_t stack_info::get_free_stack_memory_size() const
{
    return get_stack_size() - get_current_stack_position();
}

} // namespace om
