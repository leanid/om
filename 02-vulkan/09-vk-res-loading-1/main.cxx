#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_vulkan.h>

#include "experimental/scope"

import std;
import log;
import vulkan_args_parser;
import vulkan_platform_sdl3;
import vulkan_render;
import vulkan_hpp;

int main(int argc, char** argv)
{
    using namespace om;

    std::ios_base::sync_with_stdio(false); // faster iostream work and we don't
                                           // need to sync with cstdio

    std::cerr.exceptions(std::ios_base::failbit | std::ios_base::badbit);

    vulkan::args_parser args_parser(argc, argv);

    if (!args_parser.help.empty())
    {

        std::cout << "usage: " << args_parser.help << '\n';
        return 0;
    }

    bool verbose               = args_parser.verbose;
    bool vk_validation_layer   = args_parser.validation_layer;
    bool vk_debug_callback_ext = args_parser.debug_callback;

    if (verbose)
    {
        om::cout.rdbuf(std::clog.rdbuf());
    }

    if (vk_validation_layer)
    {
        om::cout << "enable vulkan validation layers\n";
        if (!SDL_SetHint(SDL_HINT_RENDER_VULKAN_DEBUG, "1"))
        {
            std::cerr << SDL_GetError();
            return 1;
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << SDL_GetError();
        return 1;
    }
    om::cout << "create all subsystems\n";
    std::experimental::scope_exit quit(
        []()
        {
            SDL_Quit();
            om::cout << "destroy all subsystems\n";
        });

    if (!SDL_Vulkan_LoadLibrary(nullptr))
    {
        std::cerr << SDL_GetError();
        return 1;
    }
    om::cout << "load vulkan library\n";
    std::experimental::scope_exit unload(
        []()
        {
            SDL_Vulkan_UnloadLibrary();
            om::cout << "unload vulkan library\n";
        });

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window(
        SDL_CreateWindow("09-vk-res-loading-1", 800, 600, SDL_WINDOW_VULKAN),
        SDL_DestroyWindow);
    std::experimental::scope_exit destroy_window(
        []() { om::cout << "destroy sdl window\n"; });

    if (!window)
    {
        om::cout << "error: can't create sdl window: " << SDL_GetError()
                 << std::endl;
        return 1;
    }

    om::cout << "sdl windows created\n";

    try
    {
        using namespace om::vulkan;
        platform_sdl3 platform(window.get(), om::cout);
        render::hints hints{
            .vulkan_version = { .major = args_parser.vulkan_version_major,
                                .minor = args_parser.vulkan_version_minor },
            .verbose        = verbose,
            .enable_validation_layers  = vk_validation_layer,
            .enable_debug_callback_ext = vk_debug_callback_ext
        };
        render render(platform, hints);

        bool running = true;
        while (running)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type) // NOLINT
                {
                    case SDL_EVENT_QUIT:
                        running = false;
                        break;
                    case SDL_EVENT_KEY_DOWN:
                        if (event.key.key == SDLK_ESCAPE)
                            running = false;
                        break;
                }
            }

            render.draw();

            // running = false;
            // std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    catch (const vk::SystemError& ex)
    {
        std::cerr << "error: vk::SystemError what: [" << ex.what() << ']'
                  << "\n    code: [" << ex.code().message() << "]" << std::endl;
        std::terminate();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "error: got exception [" << ex.what() << ']' << std::endl;
    }

    return std::cerr.fail() || std::cout.fail() ? 1 : 0;
}
