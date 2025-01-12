#include "render.hxx"

#include <cstdlib>
#include <ios>
#include <iostream>
#include <memory>
#include <thread>

#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_vulkan.h>

#include "platform_sdl3.hxx"

int main(int argc, char** argv)
{
    using namespace std;

    ios_base::sync_with_stdio(false); // faster iostream work and we don't need
                                      // to sync with cstdio

    cerr.exceptions(ios_base::failbit | ios_base::badbit);

    bool verbose               = argc > 1 && argv[1] == "-v"sv;
    bool vk_validation_layer   = true;
    bool vk_debug_callback_ext = true;

    struct null_buffer final : std::streambuf
    {
        int overflow(int c) final { return c; }
    } null;

    std::ostream  null_stream(&null);
    std::ostream& log = verbose ? std::clog : null_stream;

    if (vk_validation_layer)
    {
        log << "enable vulkan validation layers\n";
        if (!SDL_SetHint(SDL_HINT_RENDER_VULKAN_DEBUG, "1"))
        {
            std::cerr << SDL_GetError();
            return EXIT_FAILURE;
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }
    log << "create all subsystems\n";
    std::experimental::scope_exit quit(
        [&log]()
        {
            SDL_Quit();
            log << "destroy all subsystems\n";
        });

    if (!SDL_Vulkan_LoadLibrary(nullptr))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }
    log << "load vulkan library\n";
    std::experimental::scope_exit unload(
        [&log]()
        {
            SDL_Vulkan_UnloadLibrary();
            log << "unload vulkan library\n";
        });

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window(
        SDL_CreateWindow("04-vk-swapchain-1", 800, 600, SDL_WINDOW_VULKAN),
        SDL_DestroyWindow);
    std::experimental::scope_exit destroy_window(
        [&log]() { log << "destroy sdl window\n"; });

    if (!window)
    {
        log << "error: can't create sdl window: " << SDL_GetError()
            << std::endl;
        return EXIT_FAILURE;
    }

    log << "sdl windows created\n";

    try
    {
        using namespace om::vulkan;
        platform_sdl3 platform(window.get(), log);
        render::hints hints{ .verbose                  = verbose,
                             .enable_validation_layers = vk_validation_layer,
                             .enable_debug_callback_ext =
                                 vk_debug_callback_ext };
        render        render(platform, hints);

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

    return std::cerr.fail() || std::cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
}
