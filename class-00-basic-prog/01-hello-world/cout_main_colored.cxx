#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string_view>

void get_terminal_size(int& width, int& height);
bool is_terminal_support_truecolor();

int main(int, char**)
{
    // This example works on Windows 10 in MinGW terminal (tested in
    // MSys2/MinGW64 shell)
    // Also tested on Fedora 35 in Gnome-Terminal.
    // On MacOS use iTerm - it support TrueColor (default Terminal only 256
    // colors even in Monterey)

    // You can do a lot more with terminal:
    // https://en.wikipedia.org/wiki/Ncurses
    using namespace std::literals;
    using namespace std;

    int screen_width  = 80; // default
    int screen_height = 40; // default

    get_terminal_size(screen_width, screen_height);

    const bool colored_terminal_supported = is_terminal_support_truecolor();

    string_view output_text = "hello world";

    if (colored_terminal_supported)
    {
        // read more here:
        // https://en.wikipedia.org/wiki/ANSI_escape_code

        // \033 - is 27 meen ESC key code
        // [48 - meen change apply to background
        // 2 - TrueColor space
        // 0;255;0 - r;g;b - color (green)
        cout << "\033[48;2;0;255;0m"; // background 24-bit (green)
        // [38 - meen change apply to foreground (text)
        // 255;0;0 - r;g;b - color (red)
        cout << "\033[38;2;255;0;0m"; // foreground 24-bit (red)
        cout << "\033[1m";            // bold text

        // erase entire display
        cout << "\033[2J";
        // move cursor to center of screen
        cout << "\033[" << (screen_height / 2) << ";"
             << (screen_width / 2 - output_text.size() / 2) << "H";
    }

    cout << output_text;

    if (colored_terminal_supported)
    {
        // move cursor to last line
        cout << "\033[" << screen_height << ";1H";
        // finish play with terminal
        cout << "\033[0m"; // reset to default
        cout << "\x1b[0m"; // this is the same as abome! You know why?
        char array[] = { 27, 91, 48, 109, 0 };
        cout << array; // this is the same too! You know what is ASCII is?
    }

    cout << endl; // flush to device from internal buffer
    return cout.good() ? 0 : 1;
}

void get_terminal_size(int& width, int& height)
{
    using namespace std;
    char tmp_array[L_tmpnam];
    if (tmpnam(tmp_array) == nullptr)
    {
        cerr << "error can't generate tmp name\n";
    }
    string cmd_get_terminal_size = "stty size > ";
    cmd_get_terminal_size.append(tmp_array);

    system(cmd_get_terminal_size.c_str());

    // expected file with content like: xxx yyy
    // where xxx - rows, yyy - columns in decimal format
    ifstream file_with_terminal_size(tmp_array);
    file_with_terminal_size >> height >> width;
    file_with_terminal_size.close();

    remove(tmp_array); // remove tmp file
}

bool is_terminal_support_truecolor()
{
    using namespace std;
    using namespace std::literals;

    const char* term       = getenv("TERM");
    const char* color_term = getenv("COLORTERM");

    const bool colored_terminal_supported =
        (term != nullptr && term == "xterm"sv) ||
        (color_term != nullptr && color_term == "truecolor"sv);

    if (!colored_terminal_supported)
    {
        cerr << "unsupported terminal - disable colored output\n";
        cerr << "TERM=" << (term != nullptr ? term : "") << '\n';
        cerr << "COLORTERM=" << (color_term != nullptr ? color_term : "")
             << '\n';
    }
    return colored_terminal_supported;
}
