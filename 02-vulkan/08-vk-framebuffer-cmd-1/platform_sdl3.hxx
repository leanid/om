#pragma once

#include "render.hxx"

#include <SDL3/SDL.h>

namespace om::vulkan
{
struct platform_sdl3 final : platform_interface
{
    explicit platform_sdl3(SDL_Window* window, std::ostream& logger) noexcept
        : window{ window }
        , log{ logger }
    {
    }

    extensions   get_vulkan_extensions() noexcept override;
    VkSurfaceKHR create_vulkan_surface(
        VkInstance instance, VkAllocationCallbacks* alloc_callbacks) override;
    void destroy_vulkan_surface(
        VkInstance             instance,
        VkSurfaceKHR           surface,
        VkAllocationCallbacks* alloc_callbacks) noexcept override;
    buffer_size get_window_buffer_size() override;

    std::ostream& get_logger() noexcept override;

    content get_file_content(std::string_view path) override;

private:
    SDL_Window*   window;
    std::ostream& log;
};
} // namespace om::vulkan
