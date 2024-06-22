#pragma once

#include "SDL3/SDL_video.h"
#include "gfx.hxx"

#include <SDL3/SDL.h>

namespace om::vulkan
{
struct platform_sdl3 : platform_interface
{
    explicit platform_sdl3(SDL_Window* window)
        : window{ window }
    {
    }
    extensions   get_extensions() override;
    VkSurfaceKHR create_surface(
        VkInstance instance, VkAllocationCallbacks* alloc_callbacks) override;
    buffer_size get_windows_buffer_size() override;

private:
    SDL_Window* window;
};
} // namespace om::vulkan
