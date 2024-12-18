#include <algorithm>
#include <iostream>
#include <sstream>
#include <string_view>

namespace om
{

template <typename S>
std::basic_string<typename S::value_type> wrap_lines(const S&    text,
                                                     std::size_t width)
{
    using C = typename S::value_type;
    std::basic_ostringstream<C> os;

    auto process_char =
        [&os, &width, new_line_char = C{ '\n' }, current_width = std::size_t{}](
            C ch) mutable
    {
        os << ch;

        if (ch == new_line_char)
        {
            current_width = 0;
        }
        else if (current_width >= width)
        {
            os << new_line_char;
            current_width = 0;
        }
        else
        {
            current_width++;
        }
    };

    std::for_each(text.begin(), text.end(), process_char);

    return os.str();
}

class utf8_codepoint_counter
{
public:
    void reset()
    {
        octet_count     = 0;
        codepoint_count = 0;
        expected_octets = 0;
    }

    void next(char8_t octet)
    {
        unsigned char ch = static_cast<unsigned char>(octet);
        if (expected_octets == 0)
        {
            if (ch < 0x80)
            {
                // 1-byte character
                ++octet_count;
                ++codepoint_count;
            }
            else if ((ch >> 5) == 0x6)
            {
                // 2-byte character
                expected_octets = 1;
                ++octet_count;
            }
            else if ((ch >> 4) == 0xE)
            {
                // 3-byte character
                expected_octets = 2;
                ++octet_count;
            }
            else if ((ch >> 3) == 0x1E)
            {
                // 4-byte character
                expected_octets = 3;
                ++octet_count;
            }
            else
            {
                throw std::runtime_error("invalid UTF-8 sequence");
            }
        }
        else
        {
            if ((ch >> 6) != 0x2)
            {
                throw std::runtime_error("invalid UTF-8 sequence");
            }
            ++octet_count;
            --expected_octets;
            if (expected_octets == 0)
            {
                ++codepoint_count;
            }
        }
    }

    size_t num_octets() const { return octet_count; }

    size_t num_codepoints() const { return codepoint_count; }

private:
    size_t octet_count     = 0;
    size_t codepoint_count = 0;
    size_t expected_octets = 0;
};

template <>
std::u8string wrap_lines<std::u8string_view>(const std::u8string_view& text,
                                             std::size_t               width)
{
    std::basic_ostringstream<char> os;

    utf8_codepoint_counter line_counter{};

    auto process_octet =
        [&os, &width, &line_counter, new_line_char = char8_t{ '\n' }](
            char8_t octet) mutable
    {
        if (octet == new_line_char)
        {
            line_counter.reset();
        }
        else if (line_counter.num_codepoints() >= width)
        {
            os << static_cast<char>(new_line_char);
            line_counter.reset();
            line_counter.next(octet);
        }
        else
        {
            line_counter.next(octet);
        }
        os << static_cast<char>(octet);
    };

    std::for_each(text.begin(), text.end(), process_octet);

    auto           str    = os.str();
    std::u8string& result = reinterpret_cast<std::u8string&>(str);
    return result;
}

template <>
std::u8string wrap_lines<std::u8string>(const std::u8string& text,
                                        std::size_t          width)
{
    // if you remove current specialization, u8string will use ascii string
    // template and bug appears
    std::u8string_view text_view = text;
    return wrap_lines(text_view, width);
}

} // namespace om

int main()
{
    using namespace std;
    // string       text;
    // stringstream is;
    // cin >> is.rdbuf();
    // text                  = is.str();
    // string_view text_view = text;
    // cout << wrap_lines(text_view, 80) << endl;

    // wstring text_w = L"some\n long\n line more then 10 chars";
    // wcout << wrap_lines(text_w, 10) << endl;

    u8string_view u8view =
        u8"тут странный текст на русском языке, 37+ символов";
    u8string wraped_u8 = om::wrap_lines(u8view, 10);
    string&  u8ascii   = reinterpret_cast<string&>(wraped_u8);
    cout << u8ascii << endl;

    cout << "-------------------" << endl;

    u8string u8str_2 = u8"тут странный текст на русском языке, 37+ символов";
    u8string wraped_u8_2 = om::wrap_lines(u8str_2, 10);
    string&  u8ascii_2   = reinterpret_cast<string&>(wraped_u8_2);
    cout << u8ascii_2 << endl;

    return 0;
}
