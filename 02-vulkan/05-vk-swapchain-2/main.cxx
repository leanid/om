#include <algorithm>
#include <array>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <set>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

#include <experimental/scope> // not found on macOS

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_to_string.hpp>

namespace om
{
class gfx
{
public:
    using get_extensions_t = const char* const* (*)(uint32_t* num_extensions);
    using create_surface_t =
        std::function<VkSurfaceKHR(VkInstance, const VkAllocationCallbacks*)>;
    using get_window_size_t =
        std::function<void(uint32_t* width, uint32_t* height)>;

    struct hints_t
    {
        bool verbose;
        bool enable_validation_layers;
    };

    explicit gfx(std::ostream&           log,
                 get_extensions_t        get_instance_extensions,
                 const create_surface_t& create_vk_surface,
                 get_window_size_t       get_window_buffer_size,
                 hints_t                 h)
        : log{ log }
        , hints_{ h }
        , get_window_buffer_size_{ std::move(get_window_buffer_size) }
    {
        create_instance(get_instance_extensions);
        create_surface(create_vk_surface);
        get_physical_device();
        validate_physical_device();
        create_logical_device();
        create_swapchain();
    }

    ~gfx()
    {
        std::ranges::for_each(
            swapchain_image_views,
            [this](vk::ImageView image_view)
            { devices.logical.destroyImageView(image_view); });
        log << "vulkan swapchain image views destroyed\n";
        devices.logical.destroy(swapchain);
        log << "vulkan swapchain destroyed\n";
        devices.logical.destroy();
        log << "vulkan logical device destroyed\n";
        instance.destroy(surface);

        instance.destroy();
        log << "vulkan instance destroyed\n";
    }

private:
    void create_instance(get_extensions_t get_instance_extensions)
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
                        [this](std::string_view instance_extension)
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
    }

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
                         { return layer.layerName.data() == instance_layer; });

        if (it == available_layers.end())
        {
            log << "see: support/vulkan/install.md how to install validation "
                   "layer\n";
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

    static auto find_render_queue(
        const std::vector<vk::QueueFamilyProperties>& queue_properties,
        const vk::PhysicalDevice&                     device,
        const vk::SurfaceKHR&                         surface_to_check)
    {
        return std::ranges::find_if(
            queue_properties,
            [&device, &surface_to_check, &queue_properties](
                const vk::QueueFamilyProperties& property)
            {
                uint32_t index =
                    std::distance(queue_properties.data(), &property);
                return device.getSurfaceSupportKHR(index, surface_to_check);
            });
    }

    bool check_device_suitable(vk::PhysicalDevice& physical)
    {
        std::vector<vk::QueueFamilyProperties> queue_properties =
            physical.getQueueFamilyProperties();

        auto it = find_render_queue(queue_properties);

        bool render_queue_found = it != queue_properties.end();
        bool all_extensions_found =
            std::ranges::all_of(device_extensions,
                                [&physical](const char* extension_name)
                                {
                                    return check_device_extension_supported(
                                        physical, extension_name);
                                });
        return render_queue_found && all_extensions_found;
    }

    static bool check_device_extension_supported(
        vk::PhysicalDevice& device, std::string_view extension_name)
    {
        auto extensions = device.enumerateDeviceExtensionProperties();
        auto it         = std::ranges::find_if(
            extensions,
            [&extension_name](const auto& extension)
            { return extension.extensionName.data() == extension_name; });
        return it != extensions.end();
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

        log << "vulkan physical devises in the system:\n";
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
        auto it = find_if(physical_devices,
                          [this](vk::PhysicalDevice& physical)
                          { return check_device_suitable(physical); });
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

        vk::Bool32 is_supported = devices.physical.getSurfaceSupportKHR(
            queue_indexes.graphics_family, surface);

        if (!is_supported)
        {
            log << "error: physical device ["
                << devices.physical.getProperties().deviceName
                << "] with render queue index ["
                << queue_indexes.graphics_family
                << "] do not support SurfaceSupportKHR\n"
                << "try to find another queue index with VkSurfaceKHR\n";
            queue_indexes.presentation_family =
                get_presentation_queue_family_index(devices.physical, surface);
        }
        else
        {
            log << "render queue family with index ["
                << queue_indexes.graphics_family << "] support surfaceKHR\n";

            queue_indexes.presentation_family = queue_indexes.graphics_family;
        }

        auto queue_properties = devices.physical.getQueueFamilyProperties();

        const vk::QueueFamilyProperties& render_queue_properties =
            queue_properties.at(queue_indexes.graphics_family);

        log << "render queue found with index is: "
            << queue_indexes.graphics_family << '\n'
            << "render queue count: " << render_queue_properties.queueCount
            << '\n'
            << "presentation queue index is: "
            << queue_indexes.presentation_family << '\n';
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

        swapchain_details_t swap_chain_details =
            get_swapchain_details(devices.physical);

        log << swap_chain_details;

        if (swap_chain_details.presentation_modes.empty())
        {
            throw std::runtime_error("error: presentation_modes empty");
        }

        if (swap_chain_details.surface_formats.empty())
        {
            throw std::runtime_error("error: surface_formats empty");
        }
    }

    void create_logical_device()
    {
        std::set<uint32_t> queue_family_indexes = {
            queue_indexes.graphics_family, queue_indexes.presentation_family
        };

        std::vector<vk::DeviceQueueCreateInfo> queue_infos;

        float priorities = 1.f;
        for (uint32_t queue_index : queue_family_indexes)
        {
            vk::DeviceQueueCreateInfo device_queue_create_info;
            device_queue_create_info.queueFamilyIndex = queue_index;
            device_queue_create_info.queueCount       = 1;
            // 1 - hi, 0 - lowest
            device_queue_create_info.pQueuePriorities = &priorities;

            queue_infos.push_back(device_queue_create_info);
        }

        vk::PhysicalDeviceFeatures device_features;

        vk::DeviceCreateInfo device_create_info;
        device_create_info.queueCreateInfoCount =
            static_cast<uint32_t>(queue_infos.size());
        device_create_info.pQueueCreateInfos = queue_infos.data();
        device_create_info.enabledExtensionCount =
            static_cast<uint32_t>(device_extensions.size());
        device_create_info.ppEnabledExtensionNames = device_extensions.data();
        device_create_info.enabledLayerCount = 0; // in vk_1_1 this in instance
        device_create_info.pEnabledFeatures  = &device_features;

        devices.logical = devices.physical.createDevice(device_create_info);
        log << "logical device created\n";

        uint32_t queue_index = 0;
        render_queue = devices.logical.getQueue(queue_indexes.graphics_family,
                                                queue_index);
        log << "got render queue\n";
        presentation_queue = devices.logical.getQueue(
            queue_indexes.presentation_family, queue_index);
        log << "got presentation queue\n";
    }

    static std::string api_version_to_string(uint32_t apiVersion)
    {
        std::stringstream version;
        version << VK_VERSION_MAJOR(apiVersion) << '.'
                << VK_VERSION_MINOR(apiVersion) << '.'
                << VK_VERSION_PATCH(apiVersion);
        return version.str();
    }

    static uint32_t get_render_queue_family_index(
        const vk::PhysicalDevice& physical_device)
    {
        auto queue_properties = physical_device.getQueueFamilyProperties();
        auto it_render_queue  = find_render_queue(queue_properties);

        if (it_render_queue == queue_properties.end())
        {
            using namespace std::literals;
            throw std::runtime_error(
                "error: can't find render queue for device: "s +
                physical_device.getProperties().deviceName.data());
        }

        size_t graphics_queue_index =
            std::distance(queue_properties.cbegin(), it_render_queue);
        return static_cast<uint32_t>(graphics_queue_index);
    }

    static uint32_t get_presentation_queue_family_index(
        const vk::PhysicalDevice& physical_device,
        const vk::SurfaceKHR&     surface_to_check)
    {
        auto queue_properties = physical_device.getQueueFamilyProperties();
        auto it_render_queue  = find_render_queue(
            queue_properties, physical_device, surface_to_check);

        if (it_render_queue == queue_properties.end())
        {
            throw std::runtime_error(
                "error: can't find presentation queue family");
        }

        size_t graphics_queue_index =
            std::distance(queue_properties.cbegin(), it_render_queue);
        return static_cast<uint32_t>(graphics_queue_index);
    }

    void create_surface(const create_surface_t& create_vk_surface)
    {
        VkSurfaceKHR surfaceKHR = create_vk_surface(instance, nullptr);
        if (surfaceKHR == nullptr)
        {
            throw std::runtime_error(
                "error: can't create VkSurfaceKHR from user provided callback");
        }

        log << "vk surface KHR created\n";
        surface = surfaceKHR;
    }

    void create_swapchain()
    {
        swapchain_details_t swapchain_details =
            get_swapchain_details(devices.physical);

        // 1. choose best surface format
        vk::SurfaceFormatKHR selected_format =
            choose_best_surface_format(swapchain_details.surface_formats);

        log << "best surface format we choose: "
            << vk::to_string(selected_format.format) << ' '
            << vk::to_string(selected_format.colorSpace) << std::endl;
        // 2. choose best presentation mode
        vk::PresentModeKHR selected_presentation_mode =
            choose_best_present_mode(swapchain_details.presentation_modes);
        log << "best presentation_mode we choose: "
            << vk::to_string(selected_presentation_mode) << std::endl;
        // 3. choose swapchain image resolution
        vk::Extent2D selected_image_resolution =
            choose_best_swapchain_image_resolution(
                swapchain_details.surface_capabilities);
        log << "current windows image resolution: "
            << selected_image_resolution.width << 'x'
            << selected_image_resolution.height << std::endl;

        const auto& surface_capabilities =
            swapchain_details.surface_capabilities;

        uint32_t image_count = surface_capabilities.minImageCount + 1u;
        // if 0 - then limitless
        if (surface_capabilities.maxImageCount > 0u &&
            surface_capabilities.maxImageCount < image_count)
        {
            image_count = surface_capabilities.maxImageCount;
        }

        log << "image_count in swapchain minImageCount + 1 (triple buffering "
               "expected) clamped by "
               "maxImageCount = "
            << image_count << std::endl;

        vk::SwapchainCreateInfoKHR create_info;
        create_info.imageFormat     = selected_format.format;
        create_info.imageColorSpace = selected_format.colorSpace;
        create_info.presentMode     = selected_presentation_mode;
        create_info.imageExtent     = selected_image_resolution;
        create_info.minImageCount   = image_count;
        // number of layers for each image in chain
        create_info.imageArrayLayers = 1u;
        create_info.imageUsage       = vk::ImageUsageFlagBits::eColorAttachment;
        create_info.preTransform     = surface_capabilities.currentTransform;
        // how two windows will be composite together
        create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        create_info.clipped        = vk::True;

        // if Graphics and Presentation families are different, then swapchain
        // must let images be shared between families
        if (queue_indexes.graphics_family != queue_indexes.presentation_family)
        {
            log << "graphics_family != presentation_family use sharing_mode: "
                << vk::to_string(vk::SharingMode::eConcurrent) << std::endl;
            create_info.imageSharingMode    = vk::SharingMode::eConcurrent;
            std::array<uint32_t, 2> indexes = {
                queue_indexes.graphics_family, queue_indexes.presentation_family
            };
            create_info.pQueueFamilyIndices = indexes.data();
            create_info.queueFamilyIndexCount =
                static_cast<uint32_t>(indexes.size());
        }
        else
        {
            create_info.imageSharingMode      = vk::SharingMode::eExclusive;
            create_info.pQueueFamilyIndices   = nullptr;
            create_info.queueFamilyIndexCount = 0;
        }

        // if old swapchain been destroyed and this one replaces it, then link
        // old one to quickly hand over responsibilities
        create_info.oldSwapchain = nullptr;
        create_info.surface      = surface;

        swapchain = devices.logical.createSwapchainKHR(create_info);
        log << "vulkan swapchain created\n";

        // store for later usages
        swapchain_image_format = create_info.imageFormat;
        swapchain_image_extent = create_info.imageExtent;
        log << "store in utilities swapchain_image_format: "
            << vk::to_string(swapchain_image_format) << '\n'
            << "swapchain_image_extent: " << swapchain_image_extent.width << 'x'
            << swapchain_image_extent.height << std::endl;

        swapchain_images = devices.logical.getSwapchainImagesKHR(swapchain);
        log << "get swapchain images count: " << swapchain_images.size()
            << std::endl;

        swapchain_image_views.clear();
        std::ranges::transform(swapchain_images,
                               std::back_inserter(swapchain_image_views),
                               [this](vk::Image image) -> vk::ImageView
                               {
                                   return create_image_view(
                                       image,
                                       swapchain_image_format,
                                       vk::ImageAspectFlagBits::eColor);
                               });
        log << "create swapchain_image_views count: "
            << swapchain_image_views.size() << std::endl;
    }

    vk::Extent2D choose_best_swapchain_image_resolution(
        const vk::SurfaceCapabilitiesKHR& capabilities)
    {
        auto extent = capabilities.currentExtent;
        if (extent.width != std::numeric_limits<uint32_t>::max() &&
            extent.height != std::numeric_limits<uint32_t>::max())
        {
            log << "use extent2d from surface\n";
            // no need to clamp
            return extent;
        }

        uint32_t width{};
        uint32_t height{};

        get_window_buffer_size_(&width, &height);

        extent.width  = width;
        extent.height = height;

        log << "use extent2d from callback_get_window_buffer_size: " << width
            << 'x' << height << std::endl;

        auto clamp_extent = [](vk::Extent2D&       extent,
                               const vk::Extent2D& min_extent,
                               const vk::Extent2D& max_extent)
        {
            extent.width =
                std::clamp(extent.width, min_extent.width, max_extent.width);
            extent.height =
                std::clamp(extent.height, min_extent.height, max_extent.height);
        };

        clamp_extent(
            extent, capabilities.minImageExtent, capabilities.maxImageExtent);
        return extent;
    }

    static vk::SurfaceFormatKHR choose_best_surface_format(
        std::span<vk::SurfaceFormatKHR> formats)
    {
        vk::SurfaceFormatKHR default_format(vk::Format::eR8G8B8A8Unorm,
                                            vk::ColorSpaceKHR::eSrgbNonlinear);

        if (formats.empty())
        {
            throw std::runtime_error("empty surface formats");
        }

        if (formats.size() == 1 &&
            formats.front().format == vk::Format::eUndefined)
        {
            // this means all formats are supported!
            // so let's use our defaults
            return default_format;
        }
        // not all supported search for RGB or BGR
        vk::SurfaceFormatKHR suitable_formats[] = {
            default_format,
            { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear }
        };
        auto it =
            std::ranges::find_first_of(formats, std::span(suitable_formats));

        if (it != formats.end())
        {
            return *it;
        }

        // can't find suitable format lets try first as it is
        return formats.front();
    }

    static vk::PresentModeKHR choose_best_present_mode(
        std::span<vk::PresentModeKHR> present_modes)
    {
        if (std::ranges::contains(present_modes, vk::PresentModeKHR::eMailbox))
        {
            return vk::PresentModeKHR::eMailbox;
        }
        // guaranteed to be in any vulkan implementation
        return vk::PresentModeKHR::eFifo;
    }

    struct swapchain_details_t
    {
        vk::SurfaceCapabilitiesKHR        surface_capabilities;
        std::vector<vk::SurfaceFormatKHR> surface_formats;
        std::vector<vk::PresentModeKHR>   presentation_modes;
    };

    friend std::ostream& operator<<(std::ostream&              os,
                                    const swapchain_details_t& details)
    {
        os << "swap_chain_details:\n";
        auto& caps = details.surface_capabilities;
        os << "surface_capabilities:\n"
           << "\tMin image count: " << caps.minImageCount << "\n"
           << "\tMax image count: " << caps.maxImageCount << "\n"
           << "\tCurrent extent: " << caps.currentExtent.width << "x"
           << caps.currentExtent.height << "\n"
           << "\tMin image extent: " << caps.minImageExtent.width << "x"
           << caps.minImageExtent.height << "\n"
           << "\tMax image extent: " << caps.maxImageExtent.width << "x"
           << caps.maxImageExtent.height << "\n"
           << "\tMax image array layers: " << caps.maxImageArrayLayers << "\n"
           << "\tSupported transformation: "
           << vk::to_string(caps.currentTransform) << "\n"
           << "\tComposite alpha flags: "
           << vk::to_string(caps.supportedCompositeAlpha) << "\n"
           << "\tSupported usage flags: "
           << vk::to_string(caps.supportedUsageFlags) << "\n";

        os << "surface formats:\n";
        std::ranges::for_each(
            details.surface_formats,
            [&os](vk::SurfaceFormatKHR format)
            {
                os << "\tImage format: " << vk::to_string(format.format) << "\n"
                   << "\tColor space: " << vk::to_string(format.colorSpace)
                   << "\n";
            });
        os << "presentation modes:\n";
        std::ranges::for_each(details.presentation_modes,
                              [&os](vk::PresentModeKHR mode)
                              { os << '\t' << vk::to_string(mode) << '\n'; });
        return os;
    }

    swapchain_details_t get_swapchain_details(vk::PhysicalDevice& device)
    {
        swapchain_details_t details{};
        details.surface_capabilities =
            device.getSurfaceCapabilitiesKHR(surface);
        details.surface_formats    = device.getSurfaceFormatsKHR(surface);
        details.presentation_modes = device.getSurfacePresentModesKHR(surface);
        return details;
    }

    [[nodiscard]] vk::ImageView create_image_view(
        vk::Image            image,
        vk::Format           format,
        vk::ImageAspectFlags aspect_flags) const
    {
        vk::ImageViewCreateInfo info;
        info.image        = image;
        info.format       = format;
        info.viewType     = vk::ImageViewType::e2D;
        info.components.r = vk::ComponentSwizzle::eIdentity;
        info.components.g = vk::ComponentSwizzle::eIdentity;
        info.components.b = vk::ComponentSwizzle::eIdentity;
        info.components.a = vk::ComponentSwizzle::eIdentity;
        // subresources allow the view to view only a part of image
        auto& range          = info.subresourceRange;
        range.aspectMask     = aspect_flags; // ColorBit etc.
        range.baseMipLevel   = 0;            // start mip level to view from
        range.levelCount     = 1;            // count of mip levels
        range.baseArrayLayer = 0;            // start array level to view from
        range.layerCount     = 1;

        return devices.logical.createImageView(info);
    }

    // render external interface objects
    std::ostream&     log;
    hints_t           hints_;
    get_window_size_t get_window_buffer_size_;

    // vulkan main objects
    vk::Instance instance;

    struct
    {
        vk::PhysicalDevice physical;
        vk::Device         logical;
    } devices;

    [[maybe_unused]] vk::Queue render_queue;
    [[maybe_unused]] vk::Queue presentation_queue;
    vk::SurfaceKHR             surface; // KHR - extension
    vk::SwapchainKHR           swapchain;
    std::vector<vk::Image>     swapchain_images;
    std::vector<vk::ImageView> swapchain_image_views;

    // vulkan utilities
    vk::Format   swapchain_image_format{};
    vk::Extent2D swapchain_image_extent{};

    struct queue_family_indexes
    {
        uint32_t graphics_family     = std::numeric_limits<uint32_t>::max();
        uint32_t presentation_family = std::numeric_limits<uint32_t>::max();

        [[nodiscard]] bool is_valid() const
        {
            return graphics_family != std::numeric_limits<uint32_t>::max() &&
                   presentation_family != std::numeric_limits<uint32_t>::max();
        }
    } queue_indexes;

    const std::vector<const char*> device_extensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
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
        int overflow(int c) final { return c; }
    } null;

    std::ostream  null_stream(&null);
    std::ostream& log = verbose ? std::clog : null_stream;

    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }
    log << "create all subsystems\n";
    [[maybe_unused]] std::experimental::scope_exit quit(
        [&log]()
        {
            SDL_Quit();
            log << "destroy all subsystems\n";
        });

    if (0 != SDL_Vulkan_LoadLibrary(nullptr))
    {
        std::cerr << SDL_GetError();
        return EXIT_FAILURE;
    }
    log << "load vulkan library\n";
    [[maybe_unused]] std::experimental::scope_exit unload(
        [&log]()
        {
            SDL_Vulkan_UnloadLibrary();
            log << "unload vulkan library\n";
        });

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window(
        SDL_CreateWindow("04-vk-swapchain-1", 800, 600, SDL_WINDOW_VULKAN),
        SDL_DestroyWindow);
    [[maybe_unused]] std::experimental::scope_exit destroy_window(
        [&log]() { log << "destroy sdl window\n"; });

    if (!window)
    {
        log << "error: can't create sdl window: " << SDL_GetError()
            << std::endl;
    }

    log << "sdl windows created\n";

    try
    {
        om::gfx render(
            log,
            SDL_Vulkan_GetInstanceExtensions,
            [&window, &log](
                VkInstance                          instance,
                const struct VkAllocationCallbacks* allocator) -> VkSurfaceKHR
            {
                VkSurfaceKHR surface{};
                bool         result = SDL_Vulkan_CreateSurface(
                    window.get(), instance, allocator, &surface);
                if (!result)
                {
                    log << "error: can't create VkSurfaceKHR: "
                        << SDL_GetError() << std::endl;
                }
                return surface;
            },
            [&window](uint32_t* width, uint32_t* height)
            {
                int w{};
                int h{};
                if (0 != SDL_GetWindowSizeInPixels(window.get(), &w, &h))
                {
                    throw std::runtime_error(SDL_GetError());
                }

                if (width)
                {
                    *width = static_cast<uint32_t>(w);
                }

                if (height)
                {
                    *height = static_cast<uint32_t>(h);
                }
            },
            om::gfx::hints_t{ .verbose = verbose,
                              .enable_validation_layers =
                                  vk_enable_validation });
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    return std::cerr.fail();
}
