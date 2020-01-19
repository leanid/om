#include <cstdlib>
#include <iostream>
#include <thread>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    auto print_two_chars = [](char c0, char c1, size_t n) {
        for (size_t i = 0; i < n; ++i)
        {
            std::cout << c0;
            std::cout << c1;
        }
    };

    std::thread first(print_two_chars, 'a', 'b', 1024);
    std::thread second(print_two_chars, 'c', 'd', 1024);

    first.join();
    second.join();

    return 0;
}
