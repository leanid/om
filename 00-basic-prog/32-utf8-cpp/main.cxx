#include <algorithm>
#include <iostream>
#include <sstream>
#include <string_view>

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

class utf8_counter
{
public:
    utf8_counter()
        : octet_count_(0)
        , codepoint_count_(0)
        , expected_octets_(0)
    {
    }

    void reset()
    {
        octet_count_     = 0;
        codepoint_count_ = 0;
        expected_octets_ = 0;
    }

    void operator+=(char8_t octet)
    {
        unsigned char ch = static_cast<unsigned char>(octet);
        if (expected_octets_ == 0)
        {
            if (ch < 0x80)
            {
                // 1-byte character
                ++octet_count_;
                ++codepoint_count_;
            }
            else if ((ch >> 5) == 0x6)
            {
                // 2-byte character
                expected_octets_ = 1;
                ++octet_count_;
            }
            else if ((ch >> 4) == 0xE)
            {
                // 3-byte character
                expected_octets_ = 2;
                ++octet_count_;
            }
            else if ((ch >> 3) == 0x1E)
            {
                // 4-byte character
                expected_octets_ = 3;
                ++octet_count_;
            }
            else
            {
                throw std::runtime_error("Invalid UTF-8 sequence");
            }
        }
        else
        {
            if ((ch >> 6) != 0x2)
            {
                throw std::runtime_error("Invalid UTF-8 sequence");
            }
            ++octet_count_;
            if (--expected_octets_ == 0)
            {
                ++codepoint_count_;
            }
        }
    }

    size_t octets() const { return octet_count_; }

    size_t codepoints() const { return codepoint_count_; }

private:
    size_t octet_count_;
    size_t codepoint_count_;
    size_t expected_octets_;
};

template <>
std::u8string wrap_lines<std::u8string>(const std::u8string& text,
                                        std::size_t          width)
{
    std::basic_ostringstream<char> os;

    utf8_counter counter{};

    auto process_char =
        [&os, &width, &counter, new_line_char = char8_t{ '\n' }](
            char8_t ch) mutable
    {
        if (ch == new_line_char)
        {
            counter.reset();
        }
        else if (counter.codepoints() >= width)
        {
            os << static_cast<char>(new_line_char);
            counter.reset();
            counter += ch;
        }
        else
        {
            counter += ch;
        }
        os << static_cast<char>(ch);
    };

    std::for_each(text.begin(), text.end(), process_char);

    auto           str    = os.str();
    std::u8string& result = reinterpret_cast<std::u8string&>(str);
    return result;
}

int main()
{
    using namespace std;
    /*
    string       text;
    stringstream is;
    cin >> is.rdbuf();
    text                  = is.str();
    string_view text_view = text;
    cout << wrap_lines(text_view, 80) << endl;

    wstring text_w = L"some\n long\n line more then 10 chars";
    wcout << wrap_lines(text_w, 10) << endl;
    */
    u8string u8str     = u8"тут странный текст на русском языке, 37+ символов";
    u8string wraped_u8 = wrap_lines(u8str, 10);
    string   u8ascii   = reinterpret_cast<string&>(wraped_u8);
    cout << u8ascii << endl;

    return 0;
}
