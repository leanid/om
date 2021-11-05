#include <cstdlib>
#include <iostream>
#include <string_view>

int main(int, char**)
{
    // This example works on Windows 10 in MinGW terminal (tested in
    // MSys2/MinGW64 shell)
    using namespace std::literals;
    const char* term = std::getenv("TERM");

    const bool colored_terminal_supported =
        term != nullptr && term == "xterm"sv;

    if (colored_terminal_supported)
    {
        // read more here:
        // https://en.wikipedia.org/wiki/ANSI_escape_code

        // \033 - is 27 meen ESC key code
        // [48 - meen change apply to background
        // 2 - TrueColor space
        // 0;255;0 - r;g;b - color (green)
        std::cout << "\033[48;2;0;255;0m"; // background 24-bit (green)
        // [38 - meen change apply to foreground (text)
        // 255;0;0 - r;g;b - color (red)
        std::cout << "\033[38;2;255;0;0m"; // foreground 24-bit (red)
        std::cout << "\033[1m";            // bold text
    }
    else
    {
        std::cout << "unknown terminal - disable colored output\n";
        std::cout << "TERM=" << term << '\n';
    }

    std::cout << "hello world";

    if (colored_terminal_supported)
    {
        std::cout << "\033[0m"; // reset to default
        std::cout << "\x1b[0m"; // this is the same as abome! You know why?
        char array[] = { 27, 91, 48, 109, 0 };
        std::cout << array; // this is the same too! You know what is ASCII is?
    }

    std::cout << std::endl; // flush to device from internal buffer
    return std::cout.good() ? 0 : 1;
}
