#include <cstddef>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string_view>
#include <utility>

constexpr size_t Mb = 1024 * 1024;
constexpr size_t Gb = Mb * 1024;

/// If you change size of next array you may see error:
/// 00-basic-prog/22-no-alloc-memory/CMakeFiles/22-no-alloc-memory.dir/main.cxx.o:
/// in function `__static_initialization_and_destruction_0(int, int)':
/// main.cxx:(.text+0x169): relocation truncated to fit: R_X86_64_32 against
/// `.bss' main.cxx:(.text+0x178): relocation truncated to fit: R_X86_64_32
/// against `.bss'
/// Gb*3 - works on amd64 with g++
char init_time_memory[Gb * 3] = { 0 };

int main(int argc, char** argv)
{
    using namespace std::literals;

    std::string command;
    while (std::cin >> command)
    {
        if ("init_mem"sv == command)
        {
            std::cout << std::accumulate(
                &init_time_memory[0],
                &init_time_memory[sizeof(init_time_memory)],
                0);
            std::cout << std::endl;
        }
        if ("q"sv == command)
        {
            break;
        }
    }
    return 0;
}
