#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    const int i{};
    std::cout << "&argc = 0x" << std::hex << &argc << std::endl;
    std::cout << "&argv = 0x" << std::hex << &argv << std::endl;
    std::cout << "&i = 0x" << std::hex << &i << std::endl;

    return std::cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
}
