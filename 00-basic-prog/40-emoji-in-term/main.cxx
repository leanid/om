#include <bitset>
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

    return EXIT_SUCCESS;
}
