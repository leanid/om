#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string_view>

#include <experimental/scope>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_to_string.hpp>

namespace om
{
class vk_render
{
public:
    using callback = const char* const* (*)(uint32_t* num_extensions);
    struct hints
    {
        bool verbose;
    };

    explicit vk_render(std::ostream& log,
                       callback      get_instance_extensions,
                       hints         h)
        : log{ log }
    {
        vk::ApplicationInfo    application_info;
        vk::InstanceCreateInfo instance_create_info;
        instance_create_info.pApplicationInfo        = &application_info;
        instance_create_info.ppEnabledExtensionNames = get_instance_extensions(
            &instance_create_info.enabledExtensionCount);

        if (nullptr == instance_create_info.ppEnabledExtensionNames)
        {
            throw std::runtime_error(
                "get_instance_extensions callback return nullptr");
        }

        if (h.verbose)
        {
            log << "minimal vulkan expected extensions from "
                   "get_instance_extensions callback\n";
            std::for_each_n(instance_create_info.ppEnabledExtensionNames,
                            instance_create_info.enabledExtensionCount,
                            [&log](std::string_view instance_extension)
                            { log << instance_extension << '\n'; });
        }

        validate_expected_extensions_exists(instance_create_info, h);

        // TODO future all layers (maybe debug?)
        // instance_create_info.enabledLayerCount   = 0;
        // instance_create_info.ppEnabledLayerNames = nullptr;

        instance = vk::createInstance(instance_create_info);
    }

private:
    void validate_expected_extensions_exists(
        const vk::InstanceCreateInfo& instance_create_info, const hints& h)
    {
        uint32_t   num_extensions{};
        vk::Result result = vk::enumerateInstanceExtensionProperties(
            nullptr, &num_extensions, nullptr);

        if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error(
                "error: enumerateInstanceExtensionProperties returned: " +
                vk::to_string(result));
        }

        if (h.verbose)
        {
            log << "vulkan instance extension on this machine: ["
                << num_extensions << "]" << std::endl;
        }

        std::vector<vk::ExtensionProperties> extension_properties(
            num_extensions);

        result = vk::enumerateInstanceExtensionProperties(
            nullptr, &num_extensions, extension_properties.data());

        if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error(
                "error: second enumerateInstanceExtensionProperties "
                "returned: " +
                vk::to_string(result));
        }

        if (h.verbose)
        {
            log << "all vulkan instance extensions: \n";
            std::for_each(extension_properties.begin(),
                          extension_properties.end(),
                          [this](const vk::ExtensionProperties& extension)
                          {
                              log << std::setw(3) << extension.specVersion
                                  << ' ' << extension.extensionName << '\n';
                          });
        }
        std::for_each_n(
            instance_create_info.ppEnabledExtensionNames,
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
                    throw std::runtime_error(
                        "error: can't find vulkan instance extension: " +
                        std::string(extension));
                }
            });
    }

    std::ostream& log;
    vk::Instance  instance;
};
} // namespace om

int main(int argc, char** argv)
{
    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }
    std::experimental::scope_exit quit([]() { SDL_Quit(); });

    if (0 != SDL_Vulkan_LoadLibrary(nullptr))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }
    std::experimental::scope_exit unload([]() { SDL_Vulkan_UnloadLibrary(); });

    om::vk_render render(std::cerr,
                         SDL_Vulkan_GetInstanceExtensions,
                         om::vk_render::hints{ .verbose = true });

    return std::cerr.fail();
}
