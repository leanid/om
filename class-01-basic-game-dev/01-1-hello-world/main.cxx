#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string_view>

static std::string_view get_user_name(char**);

int main(int /*argc*/, char* /*argv*/[], char** env)
{
    using namespace std;

    string_view user_name = get_user_name(env);

    string_view output_phrase("hello,");

    cout << output_phrase << " " << user_name << endl;

    bool is_good = cout.good();

    int result = is_good ? EXIT_SUCCESS : EXIT_FAILURE;
    return result;
}
// clang-format off
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// clang-format on
static char** get_env_end(char** env)
{
    while (*env)
    {
        ++env;
    }

    return env;
}

static bool start_with(std::string_view str, std::string_view start)
{
    return str.size() >= start.size() && std::equal(begin(start), end(start), begin(str));
}

static std::string_view get_value(std::string_view key_value)
{
    auto end_it = end(key_value);
    auto it     = std::find(begin(key_value), end_it, '=');
    if (it == end_it)
    {
        return "";
    }
    ++it; // skip '='
    if (it == end_it)
    {
        return "";
    }
    return { it, static_cast<size_t>(end_it - it) };
}

static std::string_view get_user_name(char** env)
{
    using namespace std;
    using namespace std::placeholders;

    char** env_end = get_env_end(env);
    auto   it      = find_if(env, env_end, bind(start_with, _1, "USER="));

    return it != env_end ? get_value(*it) : "unknown";
}
