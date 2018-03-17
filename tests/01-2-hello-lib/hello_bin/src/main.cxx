#include <cstdlib>

#include <hello_lib.hxx>

int main(int /*argc*/, char* /*argv*/ [])
{
    bool is_good = greetings("Leanid");

    int result = is_good ? EXIT_SUCCESS : EXIT_FAILURE;
    return result;
}
