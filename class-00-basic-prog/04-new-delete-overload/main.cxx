#include <algorithm>
#include <iostream>
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

int main(int argc, char* argv[])
{
    std::cout << "next code will not allocate any memory\n";
    std::cout << "hello world " << argc << ' ' << 3.14159 << ' '
              << argv[argc - 1] << '\n';
    std::string str("next line allocate memory for string some long string not "
                    "fit into 16 bytes optimization");
    std::cout << "length of str is " << std::size(str) << '\n';
    std::vector<char> vec;
    std::cout << "prepare for 16Mb allocation!" << std::endl;
    vec.resize(1024 * 1024 * 16); // 16 Mb allocation!!!
    std::cout << "no more allocations\n";
    std::cout << str << '\n';
    std::cout << std::flush;
    return 0;
}
