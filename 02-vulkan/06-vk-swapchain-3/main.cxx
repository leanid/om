#include "gfx.hxx"

#include <memory>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

int main(int argc, char** argv)
{
    using namespace std::literals;

    bool verbose = argc > 1 && argv[1] == "-v"sv;
#ifdef NDEBUG
    bool vk_enable_validation = false;
#else
    bool vk_enable_validation = true;
#endif

    struct null_buffer : std::streambuf
    {
        int overflow(int c) final { return c; }
    } null;

    std::ostream  null_stream(&null);
    std::ostream& log = verbose ? std::clog : null_stream;

    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }
    log << "create all subsystems\n";
    [[maybe_unused]] std::experimental::scope_exit quit(
        [&log]()
        {
            SDL_Quit();
            log << "destroy all subsystems\n";
        });

    if (0 != SDL_Vulkan_LoadLibrary(nullptr))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }
    log << "load vulkan library\n";
    [[maybe_unused]] std::experimental::scope_exit unload(
        [&log]()
        {
            SDL_Vulkan_UnloadLibrary();
            log << "unload vulkan library\n";
        });

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window(
        SDL_CreateWindow("04-vk-swapchain-1", 800, 600, SDL_WINDOW_VULKAN),
        SDL_DestroyWindow);
    [[maybe_unused]] std::experimental::scope_exit destroy_window(
        [&log]() { log << "destroy sdl window\n"; });

    if (!window)
    {
        log << "error: can't create sdl window: " << SDL_GetError()
            << std::endl;
    }

    log << "sdl windows created\n";

    try
    {
        om::gfx render(
            log,
            SDL_Vulkan_GetInstanceExtensions,
            [&window, &log](
                VkInstance                          instance,
                const struct VkAllocationCallbacks* allocator) -> VkSurfaceKHR
            {
                VkSurfaceKHR surface{};
                SDL_bool     result = SDL_Vulkan_CreateSurface(
                    window.get(), instance, allocator, &surface);
                if (!result)
                {
                    log << "error: can't create VkSurfaceKHR: "
                        << SDL_GetError() << std::endl;
                }
                return surface;
            },
            [&window](uint32_t* width, uint32_t* height)
            {
                int w{};
                int h{};
                if (0 != SDL_GetWindowSizeInPixels(window.get(), &w, &h))
                {
                    throw std::runtime_error(SDL_GetError());
                }

                if (width)
                {
                    *width = static_cast<uint32_t>(w);
                }

                if (height)
                {
                    *height = static_cast<uint32_t>(h);
                }
            },
            om::gfx::hints{ .verbose                  = verbose,
                            .enable_validation_layers = vk_enable_validation });
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    return std::cerr.fail();
}
