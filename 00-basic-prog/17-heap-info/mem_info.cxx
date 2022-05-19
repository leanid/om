#include "mem_info.hxx"

#include <array>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace om::heap_info
{
static info get_process_heap_info()
{
    using namespace std;

    info   result{};
    string line;

    ifstream file("/proc/self/maps", ios::binary);

    // example: we need find and parse line like:
    // 562081819000-56208183a000 rw-p 00000000 00:00 0 [heap]
    while (getline(file, line))
    {
        if (line.find("[heap]") != string::npos)
        {
            stringstream ss(line);
            ss >> hex >> result.min;
            char c;
            ss >> c;
            assert(c == '-');
            ss >> hex >> result.max;
        }
    }

    return result;
}

info get_heap_process_segment_layout()
{
    return get_process_heap_info();
}

std::ostream& operator<<(std::ostream& stream, const info& heap_info)
{
    using namespace std;
    stream << hex << heap_info.min << '-' << hex << heap_info.max;
    return stream;
}
} // namespace om::heap_info
