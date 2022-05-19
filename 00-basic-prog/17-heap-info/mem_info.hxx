#pragma once

#include <cstdint>
#include <iosfwd>

namespace om::heap_info
{
struct info
{
    uintptr_t min;
    uintptr_t max;
};

info get_heap_process_segment_layout();

std::ostream& operator<<(std::ostream&, const info&);
} // namespace om::heap_info
