#pragma once

#include "render.hxx"

#include <SDL3/SDL.h>

namespace om::vulkan
{
struct platform_sdl3 final : platform_interface
{
    explicit platform_sdl3(SDL_Window* window, std::ostream& logger)
        : window{ window }
        , log{ logger }
    {
    }

    extensions   get_extensions() override;
    VkSurfaceKHR create_surface(
        VkInstance instance, VkAllocationCallbacks* alloc_callbacks) override;
    buffer_size get_windows_buffer_size() override;

    std::ostream& get_logger() override;

    content get_file_content(std::string_view path) override;

private:
    SDL_Window*   window;
    std::ostream& log;
};
} // namespace om::vulkan