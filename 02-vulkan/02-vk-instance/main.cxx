#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include <experimental/scope> // not found on macos

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_to_string.hpp>

namespace om
{
class gfx
{
public:
    using get_extensions_t = const char* const* (*)(uint32_t* num_extensions);
    struct hints_t
    {
        bool verbose;
    };

    explicit gfx(std::ostream&    log,
                 get_extensions_t get_instance_extensions,
                 hints_t          h)
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

        get_physical_device();
        validate_physical_device();
        create_logical_device();
    }

    ~gfx()
    {
        devices.logical.destroy();
        log << "vulkan logical device destroyed\n";

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
        std::ranges::for_each(extension_properties,

                              [this](const vk::ExtensionProperties& extension)
                              {
                                  log << std::setw(3) << extension.specVersion
                                      << ' ' << extension.extensionName << '\n';
                              });

        std::for_each_n(
            instance_create_info.ppEnabledExtensionNames,
            instance_create_info.enabledExtensionCount,
            [&extension_properties](std::string_view extension)
            {
                auto it = std::ranges::find_if(
                    extension_properties,

                    [extension](const vk::ExtensionProperties& other_extension)
                    {
                        return other_extension.extensionName.data() ==
                               extension;
                    });
                if (it == extension_properties.end())
                {
                    throw std::runtime_error(
                        "error: can't find vulkan instance extension: " +
                        std::string(extension));
                }
            });
    }

    static bool check_render_queue(const vk::QueueFamilyProperties& property)
    {
        return property.queueFlags & vk::QueueFlagBits::eGraphics &&
               property.queueCount >= 1;
    }

    static auto find_render_queue(
        const std::vector<vk::QueueFamilyProperties>& queue_properties)
    {
        return std::ranges::find_if(queue_properties, check_render_queue);
    }

    static bool check_device_suitable(vk::PhysicalDevice& physical)
    {
        std::vector<vk::QueueFamilyProperties> queue_properties =
            physical.getQueueFamilyProperties();

        auto it = find_render_queue(queue_properties);

        return it != queue_properties.end();
    }

    void get_physical_device()
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
        // find first suitable device
        auto it = std::ranges::find_if(physical_devices.begin(),
                                       physical_devices.end(),
                                       check_device_suitable);
        if (it == physical_devices.end())
        {
            throw std::runtime_error(
                "error: no physical devices found with render queue");
        }

        devices.physical = *it;

        log << "selected device: "
            << devices.physical.getProperties().deviceName << '\n';

        queue_indexes.graphics_family =
            get_render_queue_family_index(devices.physical);

        auto queue_properties = devices.physical.getQueueFamilyProperties();

        const vk::QueueFamilyProperties& render_queue =
            queue_properties.at(queue_indexes.graphics_family);

        log << "render queue found with index is: "
            << queue_indexes.graphics_family << '\n'
            << "render queue count: " << render_queue.queueCount << '\n';
    }

    void validate_physical_device()
    {
        // check properties
        // devices.physical.getProperties();
        // check features
        // devices.physical.getFeatures().geometryShader
        if (!queue_indexes.is_valid())
        {
            throw std::runtime_error("error: vulkan queue with render graphics "
                                     "capability not found");
        }
    }

    void create_logical_device()
    {
        uint32_t render_queue_index = queue_indexes.graphics_family;

        vk::DeviceQueueCreateInfo device_queue_create_info;
        device_queue_create_info.queueFamilyIndex = render_queue_index;

        device_queue_create_info.queueCount = 1;
        float priorities                    = 1.f;
        // 1 - hierst, 0 - lowest
        device_queue_create_info.pQueuePriorities = &priorities;

        vk::PhysicalDeviceFeatures device_features;

        vk::DeviceCreateInfo device_create_info;
        device_create_info.queueCreateInfoCount    = 1;
        device_create_info.pQueueCreateInfos       = &device_queue_create_info;
        device_create_info.enabledExtensionCount   = 0;
        device_create_info.ppEnabledExtensionNames = nullptr;
        device_create_info.enabledLayerCount = 0; // in vk_1_1 this in instance
        device_create_info.pEnabledFeatures  = &device_features;

        devices.logical = devices.physical.createDevice(device_create_info);
        log << "logical device created\n";

        uint32_t queue_index = 0;
        render_queue =
            devices.logical.getQueue(render_queue_index, queue_index);
        log << "got render queue\n";
    }

    std::string api_version_to_string(uint32_t apiVersion)
    {
        std::stringstream version;
        version << VK_VERSION_MAJOR(apiVersion) << '.'
                << VK_VERSION_MINOR(apiVersion) << '.'
                << VK_VERSION_PATCH(apiVersion);
        return version.str();
    }

    uint32_t get_render_queue_family_index(
        const vk::PhysicalDevice& physycal_device)
    {
        auto queue_properties = physycal_device.getQueueFamilyProperties();
        auto it_render_queue  = find_render_queue(queue_properties);

        size_t graphics_queue_index =
            std::distance(queue_properties.cbegin(), it_render_queue);
        return static_cast<uint32_t>(graphics_queue_index);
    }

    std::ostream& log;
    hints_t       hints_;

    vk::Instance instance;

    struct devices_t
    {
        vk::PhysicalDevice physical;
        vk::Device         logical;
    } devices;

    [[maybe_unused]] vk::Queue render_queue;

    struct queue_family_indexes
    {
        uint32_t graphics_family = std::numeric_limits<uint32_t>::max();

        [[nodiscard]] bool is_valid() const
        {
            return graphics_family != std::numeric_limits<uint32_t>::max();
        }
    } queue_indexes;
};
} // namespace om
// NOLINTNEXTLINE
int main(int argc, char** argv)
{
    using namespace std::literals;

    bool verbose = argc > 1 && argv[1] == "-v"sv;

    struct null_buffer : std::streambuf
    {
        int overflow(int c) override { return c; }
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

    om::gfx render(log,
                   SDL_Vulkan_GetInstanceExtensions,
                   om::gfx::hints_t{ .verbose = verbose });

    return std::cerr.fail();
}
