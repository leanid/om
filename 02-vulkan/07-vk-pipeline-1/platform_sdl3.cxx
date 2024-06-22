#include "platform_sdl3.hxx"

#include <SDL3/SDL_vulkan.h>

namespace om::vulkan
{

platform_interface::extensions platform_sdl3::get_extensions()
{
    platform_interface::extensions extensions{};

    extensions.names = SDL_Vulkan_GetInstanceExtensions(&extensions.count);

    return extensions;
}

VkSurfaceKHR platform_sdl3::create_surface(
    VkInstance instance, VkAllocationCallbacks* alloc_callbacks)
{
    VkSurfaceKHR surface{};
    SDL_bool     result =
        SDL_Vulkan_CreateSurface(window, instance, alloc_callbacks, &surface);

    if (!result)
    {
        throw std::runtime_error(SDL_GetError());
    }

    return surface;
}

platform_interface::buffer_size platform_sdl3::get_windows_buffer_size()
{
    platform_interface::buffer_size buffer_size{};

    int w{};
    int h{};

    if (0 != SDL_GetWindowSizeInPixels(window, &w, &h))
    {
        throw std::runtime_error(SDL_GetError());
    }

    buffer_size.width  = static_cast<uint32_t>(w);
    buffer_size.height = static_cast<uint32_t>(h);

    return buffer_size;
}

std::ostream& platform_sdl3::get_logger()
{
    return log;
}

} // namespace om::vulkan
