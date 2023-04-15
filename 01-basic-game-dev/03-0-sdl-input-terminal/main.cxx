#include <SDL3/SDL.h>

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    using namespace std;

    // [-nw] - key to drop window creation (for test purpuses)
    const bool no_window = (argc >= 2 && "-nw"s == argv[1]) ? true : false;

    const int init_result = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK |
                                     SDL_INIT_GAMEPAD | SDL_INIT_VIDEO);

    if (init_result != 0)
    {
        const char* err_message = SDL_GetError();
        cerr << "error: failed call SDL_Init: " << err_message << endl;
        return EXIT_FAILURE;
    }

    SDL_Window* const window =
        !no_window ? SDL_CreateWindow("title", 640, 480, SDL_WINDOW_OPENGL)
                   : nullptr;

    if (!no_window && window == nullptr)
    {
        const char* err_message = SDL_GetError();
        cerr << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    bool continue_loop = true;
    while (continue_loop)
    {
        SDL_Event sdl_event;

        while (SDL_PollEvent(&sdl_event))
        {
            switch (sdl_event.type)
            {
                case SDL_EVENT_KEY_DOWN:
                    [[fallthrough]];
                case SDL_EVENT_KEY_UP:
                    cerr << sdl_event.key.keysym.sym << endl;
                    break;
                case SDL_EVENT_QUIT:
                    continue_loop = false;
                    break;
                default:
                    break;
            }
        }
    }

    if (!no_window)
    {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();

    return EXIT_SUCCESS;
}
