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
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_to_string.hpp>

namespace om
{
class vk_render
{
public:
    using callback_get_ext = const char* const* (*)(uint32_t* num_extensions);
    struct hints
    {
        bool verbose;
        bool enable_validation_layers;
    };

    explicit vk_render(std::ostream&    log,
                       callback_get_ext get_instance_extensions,
                       hints            h)
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

        if (hints_.enable_validation_layers)
        {
            const char* layer = "VK_LAYER_KHRONOS_validation";
            validate_instance_layer_present(layer);

            instance_create_info.enabledLayerCount   = 1;
            instance_create_info.ppEnabledLayerNames = &layer;
            log << "enable layer: " << layer << '\n';
        }
        else
        {
            log << "vulkan validation layer disabled\n";
        }

        // instance_create_info.enabledExtensionCount   = 0;
        // instance_create_info.ppEnabledExtensionNames = nullptr;
        instance = vk::createInstance(instance_create_info);
        log << "vulkan instance created\n";

        get_phisical_device();
        validate_physical_device();
        create_logical_device();
    }

    ~vk_render()
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

    void validate_instance_layer_present(std::string_view instance_layer)
    {
        uint32_t   layer_count{};
        vk::Result r =
            vk::enumerateInstanceLayerProperties(&layer_count, nullptr);
        if (r != vk::Result::eSuccess)
        {
            throw std::runtime_error("error: can't get instance layers count");
        }
        std::vector<vk::LayerProperties> available_layers(layer_count);
        r = vk::enumerateInstanceLayerProperties(&layer_count,
                                                 available_layers.data());
        if (r != vk::Result::eSuccess)
        {
            throw std::runtime_error("error: can't get any instance layers");
        }

        log << "all vulkan layers count [" << layer_count << "]\n";
        log << "spec-version | impl-version | name and description\n";
        std::for_each(available_layers.begin(),
                      available_layers.end(),
                      [this](const vk::LayerProperties& layer)
                      {
                          log << api_version_to_string(layer.specVersion) << ' '
                              << layer.implementationVersion << ' '
                              << layer.layerName << " " << layer.description
                              << '\n';
                      });
        auto it =
            std::find_if(available_layers.begin(),
                         available_layers.end(),
                         [&instance_layer](const vk::LayerProperties& layer)
                         { return layer.layerName == instance_layer; });

        if (it == available_layers.end())
        {
            throw std::runtime_error("error: can't find requested layer: " +
                                     std::string(instance_layer));
        }
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
    hints         hints_;

    vk::Instance instance;

    struct devices_t
    {
        vk::PhysicalDevice physical;
        vk::Device         logical;
    } devices;

    vk::Queue render_queue;

    struct queue_family_indexes
    {
        uint32_t graphics_family = std::numeric_limits<uint32_t>::max();

        bool is_valid() const
        {
            return graphics_family != std::numeric_limits<uint32_t>::max();
        }
    } queue_indexes;
};
} // namespace om

int main(int argc, char** argv)
{
    using namespace std::literals;

    bool verbose = argc > 1 && argv[1] == "-v"sv;
#ifdef NDEBUG
    bool vk_enable_validation = false;
#else
    bool vk_enable_validation = true;
#endif

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

    try
    {
        om::vk_render render(log,
                             SDL_Vulkan_GetInstanceExtensions,
                             om::vk_render::hints{ .verbose = verbose,
                                                   .enable_validation_layers =
                                                       vk_enable_validation });
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    return std::cerr.fail();
}
