#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string_view>

#include <SDL3/SDL.h>

#pragma pack(push, 4)
struct bind
{
    SDL_Keycode      key;
    std::string_view name;
};
#pragma pack(pop)

void check_input(const SDL_Event& e)
{
    using namespace std;

    const array<::bind, 8> keys{ { { SDLK_w, "up" },
                                   { SDLK_a, "left" },
                                   { SDLK_s, "down" },
                                   { SDLK_d, "right" },
                                   { SDLK_LCTRL, "button_one" },
                                   { SDLK_SPACE, "button_two" },
                                   { SDLK_ESCAPE, "select" },
                                   { SDLK_RETURN, "start" } } };

    const auto it =
        find_if(begin(keys),
                end(keys),
                [&](const ::bind& b) { return b.key == e.key.key; });

    if (it != end(keys))
    {
        cout << it->name << ' ';
        if (e.type == SDL_EVENT_KEY_DOWN)
        {
            cout << "is pressed" << endl;
        }
        else
        {
            cout << "is released" << endl;
        }
    }
}

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;

    int compiled = SDL_VERSION;
    int linked   = SDL_GetVersion();

    if (compiled != linked)
    {
        cerr << "warning: SDL2 compiled and linked version mismatch: "
             << compiled << " " << linked << endl;
    }

    const int init_result = SDL_Init(SDL_INIT_VIDEO);
    if (init_result != 0)
    {
        const char* err_message = SDL_GetError();
        cerr << "error: failed call SDL_Init: " << err_message << endl;
        return EXIT_FAILURE;
    }

    SDL_Window* const window =
        SDL_CreateWindow("title", 640, 480, SDL_WINDOW_OPENGL);

    if (window == nullptr)
    {
        const char* err_message = SDL_GetError();
        cerr << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }
    if (window != nullptr)
    {
        // We have to create renderer cause without it
        // Window not visible on Wayland video driver
        SDL_Renderer* renderer = SDL_CreateRenderer(window, "opengl");
        if (renderer == nullptr)
        {
            cerr << SDL_GetError() << endl;
            return EXIT_FAILURE;
        }
        SDL_RenderPresent(renderer);
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
                    check_input(sdl_event);
                    break;
                case SDL_EVENT_QUIT:
                    continue_loop = false;
                    break;
                default:
                    break;
            }
        }
    }

    SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}
