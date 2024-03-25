#include <iostream>
#include <stacktrace> // --std=c++23 needed

void call_other_func()
{
    std::stacktrace trace = std::stacktrace::current();
    std::cerr << trace;
}

void some_func()
{
    call_other_func();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    using namespace std;

    some_func();

    return cout.fail();
}
