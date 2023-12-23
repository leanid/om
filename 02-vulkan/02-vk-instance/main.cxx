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
        , hints_{ h }
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

        log << "minimal vulkan expected extensions from "
               "get_instance_extensions callback\n";
        std::for_each_n(instance_create_info.ppEnabledExtensionNames,
                        instance_create_info.enabledExtensionCount,
                        [&log](std::string_view instance_extension)
                        { log << instance_extension << '\n'; });

        validate_expected_extensions_exists(instance_create_info);

        // TODO future all layers (maybe debug?)
        // instance_create_info.enabledLayerCount   = 0;
        // instance_create_info.ppEnabledLayerNames = nullptr;

        instance = vk::createInstance(instance_create_info);
        log << "vulkan instance created\n";

        get_phisical_device();
    }

    ~vk_render()
    {
        instance.destroy();

        log << "vulkan instance destroed\n";
    }

private:
    void validate_expected_extensions_exists(
        const vk::InstanceCreateInfo& instance_create_info)
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

        log << "vulkan instance extension on this machine: [" << num_extensions
            << "]\n";

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

        log << "all vulkan instance extensions: \n";
        std::for_each(extension_properties.begin(),
                      extension_properties.end(),
                      [this](const vk::ExtensionProperties& extension)
                      {
                          log << std::setw(3) << extension.specVersion << ' '
                              << extension.extensionName << '\n';
                      });

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

    void get_phisical_device()
    {
        using namespace std::ranges;
        std::vector<vk::PhysicalDevice> physical_devices =
            instance.enumeratePhysicalDevices();

        if (physical_devices.empty())
        {
            throw std::runtime_error(
                "error: no any vulkan physical device found in the system");
        }

        log << "vulkan physical deveses in the system:\n";
        for_each(physical_devices,
                 [this](const vk::PhysicalDevice& device)
                 {
                     const auto& properties = device.getProperties();

                     auto api_version =
                         api_version_to_string(properties.apiVersion);

                     log << "id: " << properties.deviceID << '\n'
                         << "device_name: " << properties.deviceName << '\n'
                         << "device_type: "
                         << vk::to_string(properties.deviceType) << '\n'
                         << "api_version: " << api_version << '\n';
                 });
    }
    std::string api_version_to_string(uint32_t apiVersion)
    {
        std::stringstream version;
        version << VK_VERSION_MAJOR(apiVersion) << '.'
                << VK_VERSION_MINOR(apiVersion) << '.'
                << VK_VERSION_PATCH(apiVersion);
        return version.str();
    }
    std::ostream& log;
    hints         hints_;
    vk::Instance  instance;

    struct devices_t
    {
        vk::PhysicalDevice devicePhysical;
        vk::Device         deviceLogical;
    } devices;
};
} // namespace om

int main(int argc, char** argv)
{
    using namespace std::literals;

    bool verbose = argc > 1 && argv[1] == "-v"sv;

    struct null_buffer : std::streambuf
    {
        int overflow(int c) { return c; }
    } null;

    std::ostream  null_stream(&null);
    std::ostream& log = verbose ? std::clog : null_stream;

    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }
    log << "create all subsystems\n";
    std::experimental::scope_exit quit(
        [&log]()
        {
            SDL_Quit();
            log << "destory all subsystems\n";
        });

    if (0 != SDL_Vulkan_LoadLibrary(nullptr))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }
    log << "load vulkan library\n";
    std::experimental::scope_exit unload(
        [&log]()
        {
            SDL_Vulkan_UnloadLibrary();
            log << "unload vulkan library\n";
        });

    om::vk_render render(log,
                         SDL_Vulkan_GetInstanceExtensions,
                         om::vk_render::hints{ .verbose = verbose });

    return std::cerr.fail();
}
