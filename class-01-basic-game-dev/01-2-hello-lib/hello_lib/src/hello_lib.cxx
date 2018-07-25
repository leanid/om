#include <iostream>
#include <string_view>

bool greetings(std::string_view user_name)
{
    using namespace std;

    string_view output_phrase("Hello, ");

    cout << output_phrase << user_name << endl;

    return cout.good();
}
