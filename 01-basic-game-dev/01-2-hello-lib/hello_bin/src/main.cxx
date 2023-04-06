#include <cstdlib>

#include <hello_lib.hxx>

int main(int /*argc*/, char* /*argv*/[], char* /*argv_env*/[])
{
    const char*      user_env_var = std::getenv("USER");
    std::string_view user         = user_env_var != nullptr
                                        ? user_env_var
                                        : "USER - environment variable not found";

    bool is_good = greetings(user);

    int result = is_good ? EXIT_SUCCESS : EXIT_FAILURE;
    return result;
}
