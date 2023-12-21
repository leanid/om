#include "SDL_error.h"
#include "SDL_init.h"
#include <cstdint>
#include <iostream>

#include <stdlib.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace om
{
class vk_render
{
public:
private:
};
} // namespace om

int main(int argc, char** argv)
{
    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }

    if (0 != SDL_Vulkan_LoadLibrary(nullptr))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }

    vk::ApplicationInfo    application_info;
    vk::InstanceCreateInfo instance_create_info;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.ppEnabledExtensionNames =
        SDL_Vulkan_GetInstanceExtensions(
            &instance_create_info.enabledExtensionCount);

    std::cout << "minimal extensions from SDL helper\n";
    for (unsigned i = 0; i < instance_create_info.enabledExtensionCount; ++i)
    {
        std::cout << instance_create_info.ppEnabledExtensionNames[i] << '\n';
    }

    uint32_t num_extensions{};
    std::ignore = vk::enumerateInstanceExtensionProperties(
        nullptr, &num_extensions, nullptr);
    std::cout << "num of vulkan extension on my machine: [" << num_extensions
              << "]" << std::endl;

    // TODO future all layers (maybe debug?)
    instance_create_info.enabledLayerCount   = 0;
    instance_create_info.ppEnabledLayerNames = nullptr;

    vk::Instance instance = vk::createInstance(instance_create_info);
    // instance.
    return std::cout.fail();
}
