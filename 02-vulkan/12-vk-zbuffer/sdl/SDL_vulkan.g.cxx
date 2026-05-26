
module;

#include <SDL3/SDL_vulkan.h>
#include <type_traits>

export module sdl.vulkan;

import std;

#define REGULAR_ENUM(ty)                                                       \
    constexpr bool operator==(std::underlying_type_t<ty> a, ty b) noexcept     \
    {                                                                          \
        return a == static_cast<std::underlying_type_t<ty>>(b);                \
    }                                                                          \
    constexpr bool operator==(ty a, std::underlying_type_t<ty> b) noexcept     \
    {                                                                          \
        return static_cast<std::underlying_type_t<ty>>(a) == b;                \
    }

#define BITFLAG_ENUM(ty)                                                       \
    constexpr ty operator|(ty a, ty b) noexcept                                \
    {                                                                          \
        return static_cast<ty>(static_cast<std::underlying_type_t<ty>>(a) |    \
                               static_cast<std::underlying_type_t<ty>>(b));    \
    }                                                                          \
    constexpr ty operator&(ty a, ty b) noexcept                                \
    {                                                                          \
        return static_cast<ty>(static_cast<std::underlying_type_t<ty>>(a) &    \
                               static_cast<std::underlying_type_t<ty>>(b));    \
    }                                                                          \
    constexpr ty operator^(ty a, ty b) noexcept                                \
    {                                                                          \
        return static_cast<ty>(static_cast<std::underlying_type_t<ty>>(a) ^    \
                               static_cast<std::underlying_type_t<ty>>(b));    \
    }                                                                          \
    constexpr ty operator~(ty a) noexcept                                      \
    {                                                                          \
        return static_cast<ty>(~static_cast<std::underlying_type_t<ty>>(a));   \
    }                                                                          \
    constexpr ty& operator|=(ty& a, ty b) noexcept                             \
    {                                                                          \
        return a = a | b;                                                      \
    }                                                                          \
    constexpr ty& operator&=(ty& a, ty b) noexcept                             \
    {                                                                          \
        return a = a & b;                                                      \
    }                                                                          \
    constexpr ty& operator^=(ty& a, ty b) noexcept                             \
    {                                                                          \
        return a = a ^ b;                                                      \
    }

export namespace sdl::vulkan
{

bool Vulkan_LoadLibrary(const char* path)
{
    return SDL_Vulkan_LoadLibrary(path);
}

SDL_FunctionPointer Vulkan_GetVkGetInstanceProcAddr()
{
    return SDL_Vulkan_GetVkGetInstanceProcAddr();
}

void Vulkan_UnloadLibrary()
{
    SDL_Vulkan_UnloadLibrary();
}

using ::VkAllocationCallbacks;
using ::VkInstance;
using ::VkSurfaceKHR;

bool Vulkan_CreateSurface(SDL_Window*                         window,
                          VkInstance                          instance,
                          const struct VkAllocationCallbacks* allocator,
                          VkSurfaceKHR*                       surface)
{
    return SDL_Vulkan_CreateSurface(window, instance, allocator, surface);
}

void Vulkan_DestroySurface(VkInstance                          instance,
                           VkSurfaceKHR                        surface,
                           const struct VkAllocationCallbacks* allocator)
{
    SDL_Vulkan_DestroySurface(instance, surface, allocator);
}

bool Vulkan_GetPresentationSupport(VkInstance       instance,
                                   VkPhysicalDevice physicalDevice,
                                   Uint32           queueFamilyIndex)
{
    return SDL_Vulkan_GetPresentationSupport(
        instance, physicalDevice, queueFamilyIndex);
}

const char* const* Vulkan_GetInstanceExtensions(std::uint32_t& count)
{
    return SDL_Vulkan_GetInstanceExtensions(&count);
}
} // namespace sdl::vulkan
