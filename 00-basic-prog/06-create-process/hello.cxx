#include <iostream>

int main(int, char*[])
{
    std::cout << "hello world" << std::flush;
    return std::cout.fail();
}
