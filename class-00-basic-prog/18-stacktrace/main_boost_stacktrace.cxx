#include <boost/stacktrace.hpp>
#include <iostream>

void some_other_func()
{
    std::cerr << '[' << std::endl;
    auto trace = boost::stacktrace::stacktrace();
    std::cerr << trace;
    std::cerr << ']';
}
void some_func_one()
{
    some_other_func();
}

int main(int argc, char** argv)
{
    some_func_one();
    return 0;
}
