#include <cstdlib>
#include <iostream>

#include <SDL2/SDL.h>

std::ostream& operator<<(std::ostream& out, const SDL_version& v)
{
    out << static_cast<int>(v.major) << '.';
    out << static_cast<int>(v.minor) << '.';
    out << static_cast<int>(v.patch);
    return out;
}

int main(int /*argc*/, char* /*argv*/ [])
{
    using namespace std;

    SDL_version compiled = { 0, 0, 0 };
    SDL_version linked   = { 0, 0, 0 };

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    if (SDL_COMPILEDVERSION != SDL_VERSIONNUM(linked.major, linked.minor, linked.patch))
    {
    	cerr << "warning: SDL2 compiled and linked version mismatch: " << compiled << " " << linked << endl;
    }

    const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
    if (init_result != 0)
    {
    	const char* err_message = SDL_GetError();
    	cerr << "error: failed call SDL_Init: " << err_message << endl;
    	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
