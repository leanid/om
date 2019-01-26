#include <iostream>

int main(int, char**)
{
    std::cout << "hello world" << std::endl;
    return std::cout.good() ? 0 : 1;
}
