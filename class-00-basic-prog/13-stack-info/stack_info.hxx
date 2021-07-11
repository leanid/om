#pragma once

#include <cstddef>

namespace om
{
class stack_info
{
public:
    stack_info();

    size_t get_stack_size() const;
    size_t get_current_stack_position() const;
    size_t get_free_stack_memory_size() const;

private:
    size_t address_min;
    size_t address_max;
};
} // namespace om
