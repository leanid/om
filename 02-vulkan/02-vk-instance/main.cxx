#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>

#include <source_location>
#include <stdlib.h>
#include <string_view>
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

    uint32_t   num_extensions{};
    vk::Result result = vk::enumerateInstanceExtensionProperties(
        nullptr, &num_extensions, nullptr);

    std::cout << "result=" << result
              << " num of vulkan extension on my machine: [" << num_extensions
              << "]" << std::endl;

    std::vector<vk::ExtensionProperties> extension_properties(num_extensions);

    result = vk::enumerateInstanceExtensionProperties(
        nullptr, &num_extensions, extension_properties.data());

    assert(result == vk::Result::eSuccess);

    std::for_each(extension_properties.begin(),
                  extension_properties.end(),
                  [](const vk::ExtensionProperties& extension)
                  {
                      std::cout << std::setw(3) << extension.specVersion << ' '
                                << extension.extensionName << '\n';
                  });

    // TODO future all layers (maybe debug?)
    instance_create_info.enabledLayerCount   = 0;
    instance_create_info.ppEnabledLayerNames = nullptr;

    vk::Instance instance = vk::createInstance(instance_create_info);

    // validate that extensions from SDL_Vulkan_GetInstanceExtensions
    // present in all extensions

    std::for_each(
        instance_create_info.ppEnabledExtensionNames,
        instance_create_info.ppEnabledExtensionNames +
            instance_create_info.enabledExtensionCount,
        [&extension_properties](std::string_view extension)
        {
            auto it = std::find_if(
                extension_properties.begin(),
                extension_properties.end(),
                [extension](const vk::ExtensionProperties& other_extension)
                { return other_extension.extensionName == extension; });
            if (it == extension_properties.end())
            {
                std::cerr << "error: can't find Instance Extension: "
                          << extension << '\n';
                std::exit(EXIT_FAILURE);
            }
        });

    return std::cout.fail();
}
