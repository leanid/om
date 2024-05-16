#include <cstdlib>
#include <iostream>

#include <SDL3/SDL_version.h>

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;

    int compiled = SDL_VERSION;
    int linked   = SDL_GetVersion();

    cout << "compiled: " << compiled << '\n';
    cout << "linked: " << linked << endl;

    bool is_good = cout.good();

    int result = is_good ? EXIT_SUCCESS : EXIT_FAILURE;
    return result;
}
