#include <iostream>

#include <boost/locale.hpp>

using namespace boost::locale;
int main()
{
    generator gen;
    // Create locale generator
    std::locale::global(gen(""));
    // "" - the system default locale, set
    // it globally
    const char8_t* str = u8"Привет Мир!";
    std::cout << reinterpret_cast<const char*>(str) << std::endl;
}
