#include <boost/stacktrace.hpp>
#include <boost/version.hpp>
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
    std::cerr << "boost compilation lib version: " << BOOST_LIB_VERSION
              << std::endl;
    some_func_one();
    return 0;
}
