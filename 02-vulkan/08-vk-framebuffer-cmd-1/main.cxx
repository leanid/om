#include "render.hxx"

#include <cstdlib>
#include <iostream>
#include <memory>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "platform_sdl3.hxx"

int main(int argc, char** argv)
{
    using namespace std;

    bool verbose              = argc > 1 && argv[1] == "-v"sv;
    bool vk_enable_validation = true;

    struct null_buffer final : std::streambuf
    {
        int overflow(int c) final { return c; }
    } null;

    std::ostream  null_stream(&null);
    std::ostream& log = verbose ? std::clog : null_stream;

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
                             .enable_validation_layers = vk_enable_validation };
        render        render(platform, hints);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "error: got exception [" << ex.what() << ']' << std::endl;
    }
    catch (...)
    {
        std::cerr << "unknown exception!" << std::endl;
    }

    return std::cerr.fail();
}
