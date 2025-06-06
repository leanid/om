#include <bitset>
#include <cstdint>
#include <iomanip>
#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "Можем ли мы писать по русски и использовать Эмоджи?\n";
    std::cout
        << "✅ - check mark button (code: U+2705)\n"
        << "❌ - cross mark (code: U+274C)\n"
        << "🇧🇾 - flag: Belarus (code: U+1F1E7-1F1FE)\n"
        << "🇺🇸 - flag: United States (code: U+1F1FA-1F1F8)\n"
        << "👨‍👩‍👧 - man, woman, girl (code: U+1F468 man,U+200D "
           "zero width "
           "joiner,U+1F469 woman, U+200D,U+1F467 girl)\n";
    std::string check   = "✅";
    std::string cross   = "❌";
    std::string flag_by = "🇧🇾";
    std::string flag_us = "🇺🇸";
    std::string family  = "👨‍👩‍👧";

    auto list = { check, cross, flag_by, flag_us, family };
    for (auto emoji : list)
    {
        std::cout << "emoji [" << emoji
                  << "] in utf-8 (ascii chars) length: " << emoji.length()
                  << "\n";
    }
    std::cout << "utf-8 emoji in hex\n";
    for (auto emoji : list)
    {
        std::cout << "[" << emoji << "] chars = {";

        for (char delim{ ' ' }; char ch : emoji)
        {
            std::cout << std::exchange(delim, ',') << "0x" << std::hex
                      << std::setw(2) << std::setfill('0') << (0xff & ch);
        }
        std::cout << "}\n";
    }
    std::cout << "utf-8 emoji in binary\n";
    for (auto emoji : list)
    {
        std::cout << "[" << emoji << "] chars = {";

        for (char delim{ ' ' }; char ch : emoji)
        {
            std::cout << std::exchange(delim, ',') << std::bitset<8>(ch);
        }
        std::cout << "}\n";
    }
    std::cout << "how utf-8 combined into char32_t one char?\n";
    auto bin = [](const char ch) { return std::bitset<8>(ch); };
    std::cout << "[" << check << "] chars = {" << bin(check[0]) << ","
              << bin(check[1]) << "," << bin(check[2]) << "}\n";
    std::cout << "[" << check << "] chars = {" << "____xxxx" << ","
              << "__yyyyyy" << "," << "__zzzzzz" << "}\n";

    auto utf8_to_char32 = [](const std::string_view source) noexcept -> char32_t
    {
        if (source.empty())
        {
            // Replacement Character "�"
            return U'\ufffd';
        }

        char     first_byte    = source[0];
        char32_t codepoint     = 0;
        int      bytes_to_read = 0;

        if ((first_byte & 0x80) == 0x00) // 0x80 == 0b1000'0000
        {
            // Single-byte character
            codepoint = first_byte; // 0b0xxx'xxxxx
            return codepoint;
        }
        else if ((first_byte & 0xe0) == 0xc0) // 0xe0 == 0b1110'0000
        {                                     // 0xc0 == 0b1100'0000
            // Two-byte character
            codepoint     = (first_byte & 0x1f); // 0b110x'xxxx
            bytes_to_read = 2;
        }
        else if ((first_byte & 0xf0) == 0xe0)
        {
            // Three-byte character
            codepoint     = (first_byte & 0x0f); // 0b1110'xxxx
            bytes_to_read = 3;
        }
        else if ((first_byte & 0xf8) == 0xf0)    // 0xf8 == 0b1111'1000
        {                                        // Four-byte character
            codepoint     = (first_byte & 0x07); // 0b1111'0xxx
            bytes_to_read = 4;
        }
        else
        {
            // Invalid UTF-8 sequence, return error code
            return U'\ufffd';
        }

        if (bytes_to_read > source.size())
        {
            return U'\ufffd';
        }

        for (int i = 1; i < bytes_to_read; ++i)
        {
            char nextByte = source[i];
            if ((nextByte & 0xc0) != 0x80) // 0xc0 == 0b11yy'yyyy;
            {
                // Invalid UTF-8 sequence, return error code
                return U'\ufffd'; // Replacement Character "�"
            }
            codepoint =
                (codepoint << 6) | (nextByte & 0x3F); // 0x3f == 0b0011'1111
        }
        return codepoint;
    };

    char32_t unicode_codepoint = utf8_to_char32(check);
    std::cout << "[" << check << "] char32_t = 0x" << std::hex << std::setw(8)
              << std::setfill('0') << (unicode_codepoint & 0xffffffff) << "\n";
    std::cout << "[" << check << "] char32_t = 0b"
              << std::bitset<32>(unicode_codepoint) << "\n";
    std::cout << "[" << check << "] char32_t = 0b"
              << "0000000000000000xxxxyyyyyyzzzzzz" << "\n";

    return EXIT_SUCCESS;
}
