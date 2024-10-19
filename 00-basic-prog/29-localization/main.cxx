#include <iostream>

#include <boost/locale.hpp>

template <typename Facet>
void is_facet_present(const std::locale& loc, std::ostream& os)
{
    const std::type_info& facet_id = typeid(Facet);
    if (std::has_facet<Facet>(loc))
    {
        os << "locale has facet: " << facet_id.name() << "\n";
    }
    else
    {
        os << "locale has not facet: " << facet_id.name() << "\n";
    }
}

void print_locale_properties(const std::locale& loc, std::ostream& os)
{
    os << "Locale name: " << loc.name() << '\n';

    // List of standard facets to check
    is_facet_present<std::collate<char>>(loc, os);
    is_facet_present<std::collate<wchar_t>>(loc, os);
    is_facet_present<std::ctype<char>>(loc, os);
    is_facet_present<std::ctype<wchar_t>>(loc, os);
    is_facet_present<std::codecvt<char, char, std::mbstate_t>>(loc, os);
    is_facet_present<std::codecvt<wchar_t, char, std::mbstate_t>>(loc, os);
    is_facet_present<std::moneypunct<char>>(loc, os);
    is_facet_present<std::moneypunct<wchar_t>>(loc, os);
    is_facet_present<std::money_get<char>>(loc, os);
    is_facet_present<std::money_get<wchar_t>>(loc, os);
    is_facet_present<std::money_put<char>>(loc, os);
    is_facet_present<std::money_put<wchar_t>>(loc, os);
    is_facet_present<std::numpunct<char>>(loc, os);
    is_facet_present<std::numpunct<wchar_t>>(loc, os);
    is_facet_present<std::num_get<char>>(loc, os);
    is_facet_present<std::num_get<wchar_t>>(loc, os);
    is_facet_present<std::num_put<char>>(loc, os);
    is_facet_present<std::num_put<wchar_t>>(loc, os);
    is_facet_present<std::time_get<char>>(loc, os);
    is_facet_present<std::time_get<wchar_t>>(loc, os);
    is_facet_present<std::time_put<char>>(loc, os);
    is_facet_present<std::time_put<wchar_t>>(loc, os);
};

int main()
{
    using namespace boost::locale;
    generator gen;
    // Create locale generator
    std::locale system_default = gen("");
    print_locale_properties(system_default, std::cout);

    std::locale::global(system_default);
    // "" - the system default locale, set
    // it globally
    std::cout << "true name is: " << std::boolalpha << true << std::endl;
    std::locale eng = gen("en_US.UTF-8");
    print_locale_properties(eng, std::cout);

    generator gen_limited;
    gen_limited.characters(char_facet_t::char_f);
    gen_limited.categories(category_t::collation | category_t::formatting);
    std::locale lim_de = gen_limited("de_DE.UTF-8");
    print_locale_properties(lim_de, std::cout);
    std::locale::global(lim_de);
    std::cout << "true name is: " << std::boolalpha << true << std::endl;

    const char8_t* str = u8"Привет Мир!";
    std::cout << reinterpret_cast<const char*>(str) << std::endl;
}
