#include <boost/filesystem.hpp>

#include <iostream>

int main(int, char**)
{
    std::cout << boost::filesystem::current_path() << std::endl;
    return std::cout.fail();
}
