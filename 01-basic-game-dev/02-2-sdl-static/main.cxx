#include <cstdlib>
#include <iostream>

#include <SDL3/SDL_version.h>

std::ostream& operator<<(std::ostream& out, const SDL_Version& v)
{
    out << static_cast<int>(v.major) << '.';
    out << static_cast<int>(v.minor) << '.';
    out << static_cast<int>(v.patch);
    return out;
}

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;

    SDL_Version compiled = { 0, 0, 0 };
    SDL_Version linked   = { 0, 0, 0 };

    SDL_VERSION(&compiled)
    SDL_GetVersion(&linked);

    cout << "compiled: " << compiled << '\n';
    cout << "linked: " << linked << endl;

    bool is_good = cout.good();

    int result = is_good ? EXIT_SUCCESS : EXIT_FAILURE;
    return result;
}
