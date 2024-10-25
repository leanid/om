#include <iostream>

#include <boost/core/demangle.hpp>
#include <boost/locale.hpp>

template <typename Facet>
bool is_facet_present(const std::locale& loc, std::ostream& os)
{
    const std::type_info& facet_id     = typeid(Facet);
    auto                  facet_id_str = boost::core::demangle(facet_id.name());
    if (std::has_facet<Facet>(loc))
    {
        os << "locale has facet: " << facet_id_str << "\n";
        return true;
    }
    else
    {
        os << "locale has not facet: " << facet_id_str << "\n";
    }
    return false;
}

void print_locale_properties(const std::locale& loc, std::ostream& os)
{
    using namespace std;
    if (has_facet<boost::locale::info>(loc))
    {
        auto&       info = use_facet<boost::locale::info>(loc);
        std::string tab(4, ' ');
        os << "boost::locale::info:\n"
           << tab << "name: " << info.name() << '\n'
           << tab << "country: " << info.country() << '\n'
           << tab << "encoding: " << info.encoding() << '\n'
           << tab << "language: " << info.language() << '\n'
           << tab << "utf8: " << info.utf8() << '\n'
           << tab << "variant: " << info.variant() << '\n';
    }
    else
    {
        os << "Locale name: " << loc.name() << '\n';
    }

    // List of standard facets to check
    is_facet_present<collate<char>>(loc, os);
    is_facet_present<collate<wchar_t>>(loc, os);
    is_facet_present<ctype<char>>(loc, os);
    is_facet_present<ctype<wchar_t>>(loc, os);
    is_facet_present<codecvt<char, char, std::mbstate_t>>(loc, os);
    is_facet_present<codecvt<wchar_t, char, std::mbstate_t>>(loc, os);
    is_facet_present<moneypunct<char>>(loc, os);
    is_facet_present<moneypunct<wchar_t>>(loc, os);
    is_facet_present<money_get<char>>(loc, os);
    is_facet_present<money_get<wchar_t>>(loc, os);
    is_facet_present<money_put<char>>(loc, os);
    is_facet_present<money_put<wchar_t>>(loc, os);
    if (is_facet_present<numpunct<char>>(loc, os))
    {
        auto& numpunct = use_facet<std::numpunct<char>>(loc);
        os << "decimal_point: " << numpunct.decimal_point() << '\n';
        os << "falsename: " << numpunct.falsename() << '\n';
        os << "truename: " << numpunct.truename() << '\n';
        os << "grouping: " << numpunct.grouping() << '\n';
        os << "thousands_sep: " << numpunct.thousands_sep() << '\n';
    }
    is_facet_present<numpunct<wchar_t>>(loc, os);
    is_facet_present<num_get<char>>(loc, os);
    is_facet_present<num_get<wchar_t>>(loc, os);
    is_facet_present<num_put<char>>(loc, os);
    is_facet_present<num_put<wchar_t>>(loc, os);
    is_facet_present<time_get<char>>(loc, os);
    is_facet_present<time_get<wchar_t>>(loc, os);
    is_facet_present<time_put<char>>(loc, os);
    is_facet_present<time_put<wchar_t>>(loc, os);
}

int main()
{
    using namespace boost::locale;
    using namespace std;

    localization_backend_manager my = localization_backend_manager::global();
    // Get global backend
    for (auto backend : my.get_all_backends())
    {
        cout << "boost::locale backend: " << backend << endl;
    }
    my.select("icu"); // std, icu, posix
    generator gen;
    // Create locale generator
    locale system_default = gen("");
    print_locale_properties(system_default, cout);

    locale::global(system_default);
    // "" - the system default locale, set
    // it globally
    cout << "true name is: " << boolalpha << true << endl;
    locale eng = gen("en_US.UTF-8");
    print_locale_properties(eng, cout);

    generator gen_limited;
    //    gen_limited.characters(char_facet_t::char_f);
    //   gen_limited.categories(category_t::collation | category_t::formatting);
    locale lim_de = gen_limited("fr_FR.UTF-8");
    print_locale_properties(lim_de, cout);
    cout.imbue(lim_de);
    cout << "true name is: " << std::boolalpha << true << std::endl;
    locale default_de("de_DE.UTF-8");
    cout.imbue(default_de);
    cout << "true name is: " << std::boolalpha << true << std::endl;

    std::setlocale(LC_ALL, "de_DE");
    cout.imbue(std::locale("de_DE"));
    cout << "true name is: " << std::boolalpha << true << std::endl;

    locale ru = gen("ru_RU.UTF-8");
    cout.imbue(ru);
    cout << "true name is: " << std::boolalpha << true << std::endl;

    const char8_t* str = u8"Привет Мир!";
    std::cout << reinterpret_cast<const char*>(str) << std::endl;
}
