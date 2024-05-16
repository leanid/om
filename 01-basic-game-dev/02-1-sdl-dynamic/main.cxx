#include <cstdlib>
#include <iostream>

#include <SDL3/SDL_version.h>

std::ostream& operator<<(std::ostream& out, const int& v)
{
    out << SDL_VERSIONNUM_MAJOR(v) << '.';
    out << SDL_VERSIONNUM_MINOR(v) << '.';
    out << SDL_VERSIONNUM_MINOR(v);
    return out;
}

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;

    int compiled = { 0 };
    int linked   = { 0 };

    compiled = SDL_VERSION;
    linked   = SDL_GetVersion();

    cout << "compiled: " << compiled << '\n';
    cout << "linked: " << linked << endl;

    bool is_good = cout.good();

    int result = is_good ? EXIT_SUCCESS : EXIT_FAILURE;
    return result;
}
