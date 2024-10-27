#include <iostream>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/core/demangle.hpp>
#include <boost/locale.hpp>

template <typename Facet>
bool is_facet_present(const std::locale& loc, std::ostream& os)
{
    const std::type_info& facet_id     = typeid(Facet);
    auto                  facet_id_str = boost::core::demangle(facet_id.name());
    if (std::has_facet<Facet>(loc))
    {
        os << "has: " << facet_id_str << "\n";
        return true;
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

void boost_locale_hello_example()
{
    using namespace boost::locale;
    generator   gen;
    std::locale loc = gen("");
    // Create system default locale

    std::locale::global(loc);
    // Make it system global

    std::cout.imbue(loc);
    // Set as default locale for output

    std::cout << format("Today {1,date} at {1,time} we had run our first "
                        "localization example") %
                     std::time(nullptr)
              << std::endl;

    std::cout << "This is how we show numbers in this locale " << as::number
              << 103.34 << std::endl;
    std::cout << "This is how we show currency in this locale " << as::currency
              << 103.34 << std::endl;
    std::cout << "This is typical date in the locale " << as::date
              << std::time(nullptr) << std::endl;
    std::cout << "This is typical time in the locale " << as::time
              << std::time(nullptr) << std::endl;
    std::cout << "This is upper case " << to_upper("Hello World!") << std::endl;
    std::cout << "This is lower case " << to_lower("Hello World!") << std::endl;
    std::cout << "This is title case " << to_title("Hello World!") << std::endl;
    std::cout << "This is fold case " << fold_case("Hello World!") << std::endl;
}

void boost_locale_conversion_example()
{
    if (boost::locale::localization_backend_manager::global()
            .get_all_backends()
            .at(0) != "icu")
        std::cout << "Need ICU support for this example!\nConversion below "
                     "will likely be wrong!\n";

    // Create system default locale
    boost::locale::generator gen;
    std::locale              loc = gen("");
    std::locale::global(loc);
    std::cout.imbue(loc);

    // This is needed to prevent the C stdio library from
    // converting strings to narrow on some platforms
    std::ios_base::sync_with_stdio(false);

    std::cout << "Correct case conversion can't be done by simple, character "
                 "by character conversion\n";
    std::cout << "because case conversion is context sensitive and not a "
                 "1-to-1 conversion.\n";
    std::cout << "For example:\n";
    const std::string gruessen("grüßen");
    std::cout << "   German " << gruessen
              << " would be incorrectly converted to "
              << boost::to_upper_copy(gruessen);
    std::cout << ", while Boost.Locale converts it to "
              << boost::locale::to_upper(gruessen) << std::endl
              << "     where ß is replaced with SS.\n";
    const std::string greek("ὈΔΥΣΣΕΎΣ");
    std::cout << "   Greek " << greek << " would be incorrectly converted to "
              << boost::to_lower_copy(greek);
    std::cout << ", while Boost.Locale correctly converts it to "
              << boost::locale::to_lower(greek) << std::endl
              << "     where Σ is converted to σ or to ς, according to "
                 "position in the word.\n";
    std::cout << "Such type of conversion just can't be done using "
                 "std::toupper/boost::to_upper* that work on character "
                 "by character base.\n"
                 "Also std::toupper is not fully applicable when working with "
                 "variable character length like UTF-8 or UTF-16\n"
                 "limiting the correct behavior to BMP or ASCII only\n";
}

void boost_locale_boundary_example()
{
    using namespace boost::locale;

    generator gen;
    // Make system default locale global
    std::locale loc = gen("");
    // We need the boundary facet, currently only available via ICU
    if (!std::has_facet<boundary::boundary_indexing<char>>(loc))
    {
        std::cout << "boundary detection not implemented in this environment\n";
        return;
    }
    std::locale::global(loc);
    std::cout.imbue(loc);

    std::string text = "Hello World! あにま! Linux2.6 and Windows7 is word and "
                       "number. שָלוֹם עוֹלָם!";

    std::cout << text << std::endl;

    boundary::ssegment_index index(boundary::word, text.begin(), text.end());

    for (boundary::ssegment_index::iterator p = index.begin(), e = index.end();
         p != e;
         ++p)
    {
        std::cout << "Part [" << *p << "] has ";
        if (p->rule() & boundary::word_number)
            std::cout << "number(s) ";
        if (p->rule() & boundary::word_letter)
            std::cout << "letter(s) ";
        if (p->rule() & boundary::word_kana)
            std::cout << "kana character(s) ";
        if (p->rule() & boundary::word_ideo)
            std::cout << "ideographic character(s) ";
        if (p->rule() & boundary::word_none)
            std::cout << "no word characters";
        std::cout << std::endl;
    }

    index.map(boundary::character, text.begin(), text.end());

    for (const boundary::ssegment& p : index)
        std::cout << "|" << p;
    std::cout << "|\n\n";

    index.map(boundary::line, text.begin(), text.end());

    for (const boundary::ssegment& p : index)
        std::cout << "|" << p;
    std::cout << "|\n\n";

    index.map(boundary::sentence, text.begin(), text.end());

    for (const boundary::ssegment& p : index)
        std::cout << "|" << p;
    std::cout << "|\n\n";
}

void std_locale_messages_facet_example()
{
    // on my Fedora by default
    // /usr/share/locale/ru/LC_MESSAGES/sed.mo
    {
        std::locale loc("de_DE.utf8");

        std::cout.imbue(loc);
        auto& facet = std::use_facet<std::messages<char>>(loc);
        auto  cat   = facet.open("sed", loc);
        if (cat < 0)
            std::cout << "Could not open german \"sed\" message catalog\n";
        else
            std::cout << "\"No match\" in German: "
                      << facet.get(cat, 0, 0, "No match") << '\n'
                      << "\"Memory exhausted\" in German: "
                      << facet.get(cat, 0, 0, "Memory exhausted") << '\n';
        facet.close(cat);
    }

    {
        std::locale ru("ru_RU.UTF8");

        std::cout.imbue(ru);
        auto& facet = std::use_facet<std::messages<char>>(ru);
        auto  cat   = facet.open("sed", ru);
        if (cat < 0)
            std::cout << "Could not open german \"sed\" message catalog\n";
        else
            std::cout << "\"No match\" in Russian: "
                      << facet.get(cat, 0, 0, "No match") << '\n'
                      << "\"Memory exhausted\" in Russian: "
                      << facet.get(cat, 0, 0, "Memory exhausted") << '\n';
        facet.close(cat);
    }
}

int main()
{
    boost_locale_hello_example();
    boost_locale_conversion_example();
    boost_locale_boundary_example();

    std_locale_messages_facet_example();

    using namespace std;
    std::locale default_cxx = locale("");
    std::locale default_ru  = locale("ru_RU.UTF-8");
    std::locale default_en  = locale("en_US.UTF-8");

    print_locale_properties(default_cxx, cout);
    print_locale_properties(default_ru, cout);
    print_locale_properties(default_en, cout);

    cout << "c++default locale name: " << default_cxx.name() << endl;
    namespace bl = boost::locale;
    bl::localization_backend_manager my =
        bl::localization_backend_manager::global();
    // Get global backend
    for (auto backend : my.get_all_backends())
    {
        cout << "boost::locale backend: " << backend << endl;
    }
    my.select("icu"); // std, icu, posix
    bl::generator gen;
    // Create locale generator
    std::locale system_default = gen("");
    print_locale_properties(system_default, cout);

    locale::global(system_default);
    // "" - the system default locale, set
    // it globally
    cout << "true name is: " << boolalpha << true << endl;
    locale eng = gen("en_US.UTF-8");
    print_locale_properties(eng, cout);

    bl::generator gen_limited;
    //  gen_limited.characters(char_facet_t::char_f);
    //  gen_limited.categories(category_t::collation | category_t::formatting);
    std::locale lim_de = gen_limited("fr_FR.UTF-8");
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
