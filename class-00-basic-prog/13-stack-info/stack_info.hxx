#pragma once

#include <cstddef>

namespace om
{
/// This is payground.
/// Tested on Windows 10
/// On Linux g++ supports -fsplit-stack (unlimited stack size)
/// http://gcc.gnu.org/wiki/SplitStacks On Linux you can find default stack size
/// limint by: ulimit -s - result in kB example: 8192 (on my Fedora 34 - 8MB)
class stack_info
{
public:
    stack_info();

    size_t get_stack_size() const;
    size_t get_current_stack_position() const;
    size_t get_free_stack_memory_size() const;

    size_t get_address_min() const { return address_min; }
    size_t get_address_max() const { return address_max; }

private:
    /// do not let create this object on heap
    void* operator new(size_t);
    void  operator delete(void*);

    size_t address_min;
    size_t address_max;
};
} // namespace om
