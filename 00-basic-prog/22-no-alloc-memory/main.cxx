#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <string_view>
#include <utility>

#include <sys/mman.h>

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
        if ("fill"sv == command)
        {
            std::cout << "start touching memory and write to it." << std::endl;
            std::iota(&init_time_memory[0],
                      &init_time_memory[sizeof(init_time_memory)],
                      0);

            std::cout << "random shuffle memory bytes." << std::endl;
            std::random_device rd;
            std::mt19937       g(rd());
            std::shuffle(&init_time_memory[0],
                         &init_time_memory[sizeof(init_time_memory)],
                         g);
            std::cout << "done" << std::endl;
        }
        if ("init_mem"sv == command)
        {
            std::cout << "sizeof(init_time_memory) == "sv
                      << sizeof(init_time_memory) << std::endl;
            std::cout << std::accumulate(
                &init_time_memory[0],
                &init_time_memory[sizeof(init_time_memory)],
                0);
            std::cout << "done" << std::endl;
        }
        if ("huge"sv == command)
        {
            std::cout << "try to allocate huge page like 2Mb" << std::endl;

            // This will allocate a single normal 4K page (yes we can in same
            // app on Linux use as 4k pages and Huge Pages 2Mb, 1Gb, 2Gb if
            // HugePage enabled)
            void* b = mmap(nullptr,
                           4096,
                           PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS,
                           -1,
                           0);
            if (b == MAP_FAILED)
            {
                perror("mmap failed to allocate 4k page");
                return 1;
            }

            int    flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB;
            size_t size  = 2 * Mb; // 2MB, which is the common huge page size

            void* addr =
                mmap(nullptr, size, PROT_READ | PROT_WRITE, flags, -1, 0);
            if (addr == MAP_FAILED)
            {
                std::cerr << "mmap failed to allocate 2Mb huge page\n";
                perror("mmap");
                return 1;
            }

            // use memory...
            char* ptr = static_cast<char*>(addr);
            std::fill_n(ptr, size, 'Z');

            if (munmap(addr, size) != 0)
            {
                std::cerr << "munmap failed\n";
                return 1;
            }
        }
        if ("q"sv == command)
        {
            break;
        }
    }
    return 0;
}
