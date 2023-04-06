#include <iostream>

/// NDEBUG - is standard macro
/// it should be defined in release builds
/// cmake add it for us in Release, RelWithDebInfo configurations
/// more info: https://en.cppreference.com/w/cpp/error/assert

#if defined(NDEBUG) // No debug (release)
constexpr bool g_optimized_build = true;
#else // debug
constexpr bool g_optimized_build = false;
#endif

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "Simplify"
int                      main(int, char**)
{
    using namespace std;

    if constexpr (g_optimized_build)
    {
#pragma clang diagnostic push
#pragma ide diagnostic   ignored "UnreachableCode"
        cout << "optimized build\n";
#pragma clang diagnostic pop
    }
    else
    {
        cout << "debug build\n";
    }

    return cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
}
#pragma clang diagnostic pop
