#include "platform_sdl3.hxx"

#include <utility>

#include <SDL3/SDL_vulkan.h>

#include "read_file.hxx"

namespace om::vulkan
{

platform_interface::extensions platform_sdl3::get_vulkan_extensions()
{
    platform_interface::extensions extensions{};

    uint32_t           count{};
    const char* const* names = SDL_Vulkan_GetInstanceExtensions(&count);

    if (names == nullptr)
    {
        throw std::runtime_error(SDL_GetError());
    }

    extensions.names.reserve(count);

    std::copy_n(names, count, std::back_inserter(extensions.names));

    return extensions;
}

vk::SurfaceKHR platform_sdl3::create_vulkan_surface(
    vk::Instance instance, vk::AllocationCallbacks* alloc_callbacks)
{
    VkSurfaceKHR surface{};
    int          result = SDL_Vulkan_CreateSurface(
        window,
        static_cast<VkInstance>(instance),
        reinterpret_cast<VkAllocationCallbacks*>(alloc_callbacks),
        &surface);

    if (!result)
    {
        std::string msg = "error: can't create sdl vulkan surface: [";
        msg += SDL_GetError();
        msg += ']';
        throw std::runtime_error(msg);
    }

    return surface;
}

void platform_sdl3::destroy_vulkan_surface(
    vk::Instance             instance,
    vk::SurfaceKHR           surface,
    vk::AllocationCallbacks* alloc_callbacks) noexcept
{
    SDL_Vulkan_DestroySurface(
        static_cast<VkInstance>(instance),
        static_cast<VkSurfaceKHR>(surface),
        reinterpret_cast<VkAllocationCallbacks*>(alloc_callbacks));
}

platform_interface::buffer_size platform_sdl3::get_window_buffer_size()
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

std::ostream& platform_sdl3::get_logger() noexcept
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
