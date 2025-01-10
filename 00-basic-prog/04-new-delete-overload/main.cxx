#include <algorithm>
#include <cstring>
#include <iostream>
#include <numbers>
#include <sstream>
#include <vector>

void* operator new(size_t n) noexcept(false)
{
    void* p = ::malloc(n);
    std::cout << "new " << p << " size " << n << '\n';
    return p;
}

void operator delete(void* p) noexcept(true)
{
    std::cout << "delete " << p << '\n';
    ::free(p);
}

void operator delete(void* p, std::size_t n) noexcept(true)
{
    std::cout << "delete " << p << " size " << n << '\n';
    ::free(p);
}
// NOLINTNEXTLINE
int main(int argc, char* argv[])
{
    std::cout
        << "-main-start------------------------------------------------------\n"
        << std::flush;
    {
        std::cout << "next line will not allocate any memory\n" << std::flush;
        std::cout << "hello world " << argc << ' ' << std::numbers::pi << ' '
                  << argv[argc - 1] << '\n';
        std::string str(
            "next line allocate memory for string some long string not "
            "fit into 16 bytes optimization");
        std::cout << "length of str is " << std::size(str) << '\n';
        std::vector<char> vec;
        std::cout << "prepare for 16Mb allocation!" << std::endl;
        vec.resize(1024ull * 1024 * 16); // 16 Mb allocation!!!
        std::cout << "no more allocations\n";
        std::cout << str << '\n';
        std::cout << std::flush;
    }
    {
        std::cout << "-play-with-string-stream---------------------------------"
                     "----------------\n"
                  << std::flush;
        std::stringstream ss;
        ss << "hello world\n";
        const char* s = "asdfasdfasdfasfdasdfasfdasdfasfdasdfasfdsdfasdfasfdafd"
                        "sfasfdasdfasdfasfdfsadfafsdasfdasfdafdsasfdasfd";
        const size_t len = strlen(s);
        ss << s << " " << len << '\n';
        std::cout << ss.str() << std::flush;
    }
    {
        std::cout
            << "-play-with-array------------------------------------------\n";
        size_t arr_size = 128;
        char*  array    = new char[arr_size];
        delete[] array;
        std::cout
            << "-finish-with-array----------------------------------------\n";
    }
    std::cout << "-main-finish-------------------------------------------------"
                 "-----\n"
              << std::flush;
    return 0;
}
