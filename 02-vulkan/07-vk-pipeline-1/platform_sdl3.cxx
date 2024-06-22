#include "platform_sdl3.hxx"

#include <utility>

#include <SDL3/SDL_vulkan.h>

#include "read_file.hxx"

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

platform_interface::content platform_sdl3::get_file_content(
    std::string_view path)
{
    content     result{};
    io::content content = io::read_file(path);

    result.memory = std::move(content.memory);
    result.size   = std::exchange(content.size, 0);

    return result;
}
} // namespace om::vulkan
