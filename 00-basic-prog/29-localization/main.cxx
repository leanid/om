#include <iostream>

#include <boost/locale.hpp>

int main()
{
    using namespace boost::locale;
    generator gen;
    // Create locale generator
    std::locale::global(gen(""));
    // "" - the system default locale, set
    // it globally
    std::cout << "true name is: " << std::boolalpha << true << std::endl;
    std::locale loc = gen("en_US.UTF-8");

    generator gen_limited;
    gen_limited.characters(char_facet_t::char_f);
    gen_limited.categories(category_t::collation | category_t::formatting);
    std::locale::global(gen_limited("de_DE.UTF-8"));
    std::cout << "true name is: " << std::boolalpha << true << std::endl;

    const char8_t* str = u8"Привет Мир!";
    std::cout << reinterpret_cast<const char*>(str) << std::endl;
}
