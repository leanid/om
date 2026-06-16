#include <cstdlib>
#include <iostream>
#include <print>

int main()
{
    std::print(std::cout,
               "main_print_std23_modern.cxx hello world form c++23 "
               "#include <print>\n");
    std::cout.flush();
    return std::cout.fail();
}
