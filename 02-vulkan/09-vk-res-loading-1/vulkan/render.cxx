#include "render.hxx"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <ranges>
#include <set>
#include <sstream>
#if __has_include(<stacktrace>)
#include <stacktrace>
#else
namespace std::stacktrace
{
auto current()
{
    return "no stacktrace";
}
} // namespace std::stacktrace
#endif
#include <stdexcept>
#include <string_view>
#include <tuple>

#include "experimental/report_duration.hxx"

namespace om::vulkan
{
/// @breaf maximum number of frames to be processed simultaneously by the GPU
static constexpr size_t max_frames_in_gpu = 2;

/// @concurency note
/// A callback will always be executed in the same thread as the originating
/// Vulkan call.
///
/// A callback can be called from multiple threads simultaneously (if the
/// application is making Vulkan calls from multiple threads).
///
/// @param data note data->pMessage is NULL if messageTypes is equal to
/// VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT, or a
/// null-terminated UTF-8 string detailing the trigger conditions.
///
/// @return VkBool32 The callback returns a VkBool32, which is interpreted in a
/// layer-specified manner. The application should always return VK_FALSE. The
/// VK_TRUE value is reserved for use in layer development.
/// c++ vulkan.hpp declaration from Vulkan SDK 1.4.304
///   typedef vk::Bool32( VKAPI_PTR * PFN_DebugUtilsMessengerCallbackEXT )(
///    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
///    vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
///    const vk::DebugUtilsMessengerCallbackDataEXT * pCallbackData,
///    void * pUserData );

static VKAPI_ATTR vk::Bool32 VKAPI_CALL
debug_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT      severity,
               vk::DebugUtilsMessageTypeFlagsEXT             msg_type,
               const vk::DebugUtilsMessengerCallbackDataEXT* data,
               void*                                         user_data)
{
    using namespace std;

    const char* msg_type_name = "unknown";
    const char* msg           = data->pMessage;
    string      msg_extended;

    switch (static_cast<decltype(msg_type)::MaskType>(msg_type))
    {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            msg_type_name = "general";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            msg_type_name = "validation";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            msg_type_name = "performance";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
            msg_type_name = "device_address_binding";
            msg           = "";
            break;
        default:
            break;
    }

    const char* severity_name    = "unknown";
    bool        print_stacktrace = false;

    switch (severity)
    {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            severity_name = "verbose";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            severity_name = "info";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            severity_name = "warning";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            severity_name    = "error";
            print_stacktrace = true;
            break;
        default:
            break;
    }

    auto build_list_of_objects_info =
        [](const vk::DebugUtilsObjectNameInfoEXT* objects,
           uint32_t                               count) -> string
    {
        stringstream ss;
        for_each_n(objects,
                   count,
                   [&ss](const auto& object)
                   {
                       vk::DebugUtilsObjectNameInfoEXT object_info = object;
                       ss << "type: " << vk::to_string(object_info.objectType)
                          << ' ';
                       ss << "name: "
                          << (object_info.pObjectName ? object_info.pObjectName
                                                      : "null")
                          << ' ';
                   });
        return ss.str();
    };

    msg_extended =
        build_list_of_objects_info(data->pObjects, data->objectCount);

    cerr << "vk: [" << severity_name << "] [" << msg_type_name << "] " << msg
         << "\n    objects: " << msg_extended << endl;

    if (print_stacktrace)
    {
        cerr << std::stacktrace::current() << endl;
        std::terminate();
    }

    return VK_FALSE;
}

static std::string api_version_to_string(uint32_t apiVersion)
{
    std::stringstream version;
    version << VK_VERSION_MAJOR(apiVersion) << '.'
            << VK_VERSION_MINOR(apiVersion) << '.'
            << VK_VERSION_PATCH(apiVersion);
    return version.str();
}

render::render(platform_interface& platform, hints hints)
    : log{ platform.get_logger() }
    , platform_{ platform }
    , hints_{ hints }
{
    create_instance(hints.enable_validation_layers,
                    hints.enable_debug_callback_ext);
    create_debug_callback(hints.enable_debug_callback_ext);
    create_surface();
    get_physical_device();
    validate_physical_device();
    create_logical_device();
    // add debug names to vk objects
    set_object_name(instance, "om_main_instance");
    set_object_name(surface, "om_main_surface");
    set_object_name(devices.physical, "om_physical_device");
    set_object_name(devices.logical, "om_logical_device");

    // clang-format off
    std::vector<vertex> mesh_verticles = {
        {.pos{ 0.4f, -0.4f, 0.0f }, .col{1.0f, 0.0f, 0.0f}},
        {.pos{ 0.4f, 0.4f, 0.0f },  .col{0.0f, 1.0f, 0.0f}},
        {.pos{ -0.4f, 0.4f, 0.0f }, .col{0.0f, 0.0f, 1.0f}},

        {.pos{ -0.4f, -0.4f, 0.0f}, .col{1.0f, 1.0f, 0.0f}},
        {.pos{ 0.4f, -0.4f, 0.0f }, .col{1.0f, 0.0f, 0.0f}},
        {.pos{ -0.4f, 0.4f, 0.0f }, .col{0.0f, 0.0f, 1.0f}}
    };
    // clang-format on

    first_mesh = mesh(
        devices.physical, devices.logical, std::span{ mesh_verticles }, *this);

    create_swapchain();
    create_renderpass();
    create_graphics_pipeline();
    create_framebuffers();
    create_command_pool();
    create_command_buffers();
    record_commands();
    create_synchronization_objects();
}

render::~render()
{
    try
    {
        om::tools::report_duration duration{ log, "render::~render()" };
        {
            om::tools::report_duration duration{ log,
                                                 "devices.logical.waitIdle()" };
            devices.logical.waitIdle();
        }

        first_mesh.cleanup();

        destroy_synchronization_objects();
        log << "vulkan synchronization objects destroyed\n";
        devices.logical.freeCommandBuffers(graphics_command_pool,
                                           command_buffers);
        log << "vulkan command buffers freed\n";
        devices.logical.destroyCommandPool(graphics_command_pool);
        log << "vulkan destroy command pool\n";
        std::ranges::for_each(
            swapchain_framebuffers,
            [this](vk::Framebuffer& framebuffer)
            { devices.logical.destroyFramebuffer(framebuffer); });
        log << "vulkan framebuffers destroyed\n";
        devices.logical.destroy(graphics_pipeline);
        log << "vulkan graphics_pipeline destroyed\n";
        devices.logical.destroy(pipeline_layout);
        log << "vulkan pipeline_leyout destroyed\n";
        devices.logical.destroy(render_path);
        log << "vulkan render_path destroyed\n";
        std::ranges::for_each(
            swapchain_image_views,
            [this](vk::ImageView image_view)
            { devices.logical.destroyImageView(image_view); });
        log << "vulkan swapchain image views destroyed\n";
        devices.logical.destroy(swapchain);
        log << "vulkan swapchain destroyed\n";
        devices.logical.destroy();
        log << "vulkan logical device destroyed\n";
        destroy_surface();

        destroy_debug_callback();
        log << "vulkan debug callback destroyed\n";
        instance.destroy();
        log << "vulkan instance destroyed\n";
    }
    catch (std::exception& e)
    {
        log << "error: during render::~render() " << e.what() << std::endl;
    }
}

void render::draw()
{
    // breafly:
    // 1. Acquire available image to draw and fire semaphore when it's ready
    // 2. Submit command buffer to queue for execution. Making sure that waits
    //    for the image to be available before drawing and signals when it has
    //    finished drawing
    // 3. Present image to screen when it has signaled finished drawing

    // Get Image from swapchain
    uint32_t index_from_swapchain = std::numeric_limits<uint32_t>::max();
    {
        auto fence_op_result = devices.logical.waitForFences(
            1,
            &synchronization.gpu_fence.at(current_frame_index),
            vk::True,
            std::numeric_limits<uint64_t>::max());

        if (fence_op_result != vk::Result::eSuccess)
        {
            throw std::runtime_error("error: can't wait for fence");
        }

        fence_op_result = devices.logical.resetFences(
            1, &synchronization.gpu_fence.at(current_frame_index));

        if (fence_op_result != vk::Result::eSuccess)
        {
            throw std::runtime_error("error: can't reset fence");
        }

        auto image_index = devices.logical.acquireNextImageKHR(
            swapchain,
            std::numeric_limits<uint64_t>::max(),
            synchronization.image_available.at(current_frame_index),
            {});

        if (image_index.result != vk::Result::eSuccess)
        {
            throw std::runtime_error("error: can't acquire next image");
        }
        index_from_swapchain = image_index.value;
    }

    // Submit command buffer to queue
    {
        vk::PipelineStageFlags wait_stages =
            vk::PipelineStageFlagBits::eColorAttachmentOutput;

        auto& cmd_buffer = command_buffers.at(index_from_swapchain);

        vk::SubmitInfo submit_info{};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores =
            &synchronization.image_available.at(current_frame_index);
        submit_info.pWaitDstStageMask    = &wait_stages;
        submit_info.commandBufferCount   = 1;
        submit_info.pCommandBuffers      = &cmd_buffer;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores =
            &synchronization.render_finished.at(current_frame_index);

        render_queue.submit(
            submit_info,
            // set fence to vk::True after queue finish execution
            synchronization.gpu_fence.at(current_frame_index));
    }

    // Present image to screen
    {
        vk::PresentInfoKHR present_info{};
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores =
            &synchronization.render_finished.at(current_frame_index);
        present_info.swapchainCount = 1;
        present_info.pSwapchains    = &swapchain;
        present_info.pImageIndices  = &index_from_swapchain;

        auto result = presentation_queue.presentKHR(present_info);

        if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error("error: can't present image to screen");
        }
    }

    current_frame_index = (current_frame_index + 1) % max_frames_in_gpu;
}

void render::create_instance(bool enable_validation_layers,
                             bool enable_debug_callback_ext)
{
    vk::ApplicationInfo application_info{
        "om vulkan tutorial",
        VK_MAKE_API_VERSION(0, 0, 1, 0),
        "om",
        VK_MAKE_API_VERSION(0, 0, 1, 0),
        VK_MAKE_API_VERSION(
            0, hints_.vulkan_version.major, hints_.vulkan_version.minor, 0)
    };

    vk::InstanceCreateInfo instance_create_info;
    instance_create_info.pApplicationInfo = &application_info;

    platform_interface::extensions extensions =
        platform_.get_vulkan_extensions();

    if (extensions.names.empty())
    {
        throw std::runtime_error(
            "get_instance_extensions callback return nullptr");
    }

    log << "minimal vulkan expected extensions from "
           "platform.get_vulkan_extensions():\n";

    std::ranges::for_each(extensions.names,
                          [this](std::string_view instance_extension)
                          { log << "    - " << instance_extension << '\n'; });

    if (enable_debug_callback_ext)
    {
        extensions.names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        log << "    - " << extensions.names.back() << '\n';
    }

    instance_create_info.ppEnabledExtensionNames = extensions.names.data();
    instance_create_info.enabledExtensionCount =
        static_cast<uint32_t>(extensions.names.size());

    validate_expected_extensions_exists(instance_create_info);

    if (enable_validation_layers)
    {
        const char* layer = "VK_LAYER_KHRONOS_validation";
        try
        {
            validate_instance_layer_present(layer);

            instance_create_info.enabledLayerCount   = 1;
            instance_create_info.ppEnabledLayerNames = &layer;
            log << "enable layer: " << layer << '\n';
        }
        catch (std::exception& e)
        {
            log << e.what() << std::endl;
        }
    }
    else
    {
        log << "vulkan validation layer disabled\n";
    }

    instance = vk::createInstance(instance_create_info);

    log << "vulkan instance created\n";
    log << "vk api version: " << hints_.vulkan_version.major << '.'
        << hints_.vulkan_version.minor << " requested\n";

    log << "vk api version: " << VK_VERSION_MAJOR(application_info.apiVersion)
        << '.' << VK_VERSION_MINOR(application_info.apiVersion)
        << " after instance created\n";

    uint32_t maximum_supported = vk::enumerateInstanceVersion();
    log << "vk api version: " << VK_VERSION_MAJOR(maximum_supported) << '.'
        << VK_VERSION_MINOR(maximum_supported) << '.'
        << VK_VERSION_PATCH(maximum_supported) << " maximum supported\n";

    dynamic_loader =
        vk::detail::DispatchLoaderDynamic{ instance, vkGetInstanceProcAddr };
    log << "vulkan dynamic loader created\n";
}

void render::create_debug_callback(bool enable_debug_callback)
{
    if (!enable_debug_callback)
    {
        log << "vulkan debug callback disabled\n";
        return;
    }

    vk::DebugUtilsMessengerCreateInfoEXT debug_info;
    debug_info.messageSeverity =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    debug_info.messageType =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;
    debug_info.pfnUserCallback = debug_callback;
    debug_info.pUserData       = nullptr;

    debug_extension = instance.createDebugUtilsMessengerEXT(
        debug_info, nullptr, dynamic_loader);

    log << "vulkan debug callback created\n";
}

void render::destroy_debug_callback() noexcept
{
    if (debug_extension)
    {
        instance.destroyDebugUtilsMessengerEXT(
            debug_extension, nullptr, dynamic_loader);
        log << "vulkan debug callback destroyed\n";
    }
}

void render::validate_expected_extensions_exists(
    const vk::InstanceCreateInfo& create_info)
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

    std::vector<vk::ExtensionProperties> extension_properties(num_extensions);

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
        create_info.ppEnabledExtensionNames,
        create_info.enabledExtensionCount,
        [&extension_properties](std::string_view extension)
        {
            auto it = std::ranges::find_if(
                extension_properties,

                [extension](const vk::ExtensionProperties& other_extension)
                { return other_extension.extensionName.data() == extension; });

            if (it == extension_properties.end())
            {
                throw std::runtime_error(
                    "error: can't find vulkan instance extension: " +
                    std::string(extension));
            }
        });
}

void render::validate_instance_layer_present(std::string_view instance_layer)
{
    uint32_t   layer_count{};
    vk::Result r = vk::enumerateInstanceLayerProperties(&layer_count, nullptr);
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
    std::ranges::for_each(available_layers,

                          [this](const vk::LayerProperties& layer)
                          {
                              log << api_version_to_string(layer.specVersion)
                                  << ' ' << layer.implementationVersion << ' '
                                  << layer.layerName << " " << layer.description
                                  << '\n';
                          });
    auto it = std::ranges::find_if(
        available_layers,

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
            uint32_t index = std::distance(queue_properties.data(), &property);
            return device.getSurfaceSupportKHR(index, surface_to_check);
        });
}

bool render::check_device_suitable(vk::PhysicalDevice& physical)
{
    std::vector<vk::QueueFamilyProperties> queue_properties =
        physical.getQueueFamilyProperties();

    auto it = find_render_queue(queue_properties);

    bool render_queue_found   = it != queue_properties.end();
    bool all_extensions_found = std::ranges::all_of(
        device_extensions,
        [&physical](const char* extension_name)
        { return check_device_extension_supported(physical, extension_name); });
    return render_queue_found && all_extensions_found;
}

bool render::check_device_extension_supported(vk::PhysicalDevice& device,
                                              std::string_view extension_name)
{
    auto extensions = device.enumerateDeviceExtensionProperties();
    auto it         = std::ranges::find_if(
        extensions,
        [&extension_name](const auto& extension)
        { return extension.extensionName.data() == extension_name; });
    return it != extensions.end();
}

void render::get_physical_device()
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
                     << "device_type: " << vk::to_string(properties.deviceType)
                     << '\n'
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

    log << "selected device: " << devices.physical.getProperties().deviceName
        << '\n';

    queue_indexes.graphics_family =
        get_render_queue_family_index(devices.physical);

    vk::Bool32 is_supported = devices.physical.getSurfaceSupportKHR(
        queue_indexes.graphics_family, surface);

    if (!is_supported)
    {
        log << "error: physical device ["
            << devices.physical.getProperties().deviceName
            << "] with render queue index [" << queue_indexes.graphics_family
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

    log << "render queue found with index is: " << queue_indexes.graphics_family
        << '\n'
        << "render queue count: " << render_queue_properties.queueCount << '\n'
        << "presentation queue index is: " << queue_indexes.presentation_family
        << '\n';
}

uint32_t render::get_render_queue_family_index(
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

uint32_t render::get_presentation_queue_family_index(
    const vk::PhysicalDevice& physical_device,
    const vk::SurfaceKHR&     surface_to_check)
{
    auto queue_properties = physical_device.getQueueFamilyProperties();
    auto it_render_queue =
        find_render_queue(queue_properties, physical_device, surface_to_check);

    if (it_render_queue == queue_properties.end())
    {
        throw std::runtime_error("error: can't find presentation queue family");
    }

    size_t graphics_queue_index =
        std::distance(queue_properties.cbegin(), it_render_queue);
    return static_cast<uint32_t>(graphics_queue_index);
}

void render::create_surface()
{
    VkSurfaceKHR surfaceKHR =
        platform_.create_vulkan_surface(instance, nullptr);
    if (surfaceKHR == nullptr)
    {
        throw std::runtime_error(
            "error: can't create VkSurfaceKHR from user provided callback");
    }

    log << "vk surface KHR created\n";
    surface = surfaceKHR;
}

std::ostream& operator<<(std::ostream&                      os,
                         const render::swapchain_details_t& details)
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
       << "\tSupported transformation: " << vk::to_string(caps.currentTransform)
       << "\n"
       << "\tComposite alpha flags: "
       << vk::to_string(caps.supportedCompositeAlpha) << "\n"
       << "\tSupported usage flags: " << vk::to_string(caps.supportedUsageFlags)
       << "\n";

    os << "surface formats:\n";
    std::ranges::for_each(
        details.surface_formats,
        [&os](vk::SurfaceFormatKHR format)
        {
            os << "\tImage format: " << vk::to_string(format.format) << "\n"
               << "\tColor space: " << vk::to_string(format.colorSpace) << "\n";
        });
    os << "presentation modes:\n";
    std::ranges::for_each(details.presentation_modes,
                          [&os](vk::PresentModeKHR mode)
                          { os << '\t' << vk::to_string(mode) << '\n'; });
    return os;
}

void render::validate_physical_device()
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

void render::create_logical_device()
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
    render_queue =
        devices.logical.getQueue(queue_indexes.graphics_family, queue_index);
    log << "got render queue\n";
    presentation_queue = devices.logical.getQueue(
        queue_indexes.presentation_family, queue_index);
    log << "got presentation queue\n";
}

inline std::ostream& operator<<(std::ostream&                     os,
                                const vk::SurfaceCapabilitiesKHR& caps)
{
    os << "vk::SurfaceCapabilitiesKHR:\n";
    os << "  minImageCount: " << caps.minImageCount << "\n";
    os << "  maxImageCount: "
       << (caps.maxImageCount == 0 ? "Unlimited"
                                   : std::to_string(caps.maxImageCount))
       << "\n";
    os << "  currentExtent: {" << caps.currentExtent.width << ", "
       << caps.currentExtent.height << "}\n";
    os << "  minImageExtent: {" << caps.minImageExtent.width << ", "
       << caps.minImageExtent.height << "}\n";
    os << "  maxImageExtent: {" << caps.maxImageExtent.width << ", "
       << caps.maxImageExtent.height << "}\n";
    os << "  maxImageArrayLayers: " << caps.maxImageArrayLayers << "\n";

    // Supported transforms
    os << "  supportedTransforms: " << vk::to_string(caps.supportedTransforms)
       << "\n";

    // Current transform
    os << "  currentTransform: " << vk::to_string(caps.currentTransform)
       << "\n";

    // Supported composite alpha
    os << "  supportedCompositeAlpha: "
       << vk::to_string(caps.supportedCompositeAlpha) << "\n";

    // Supported usage flags
    os << "  supportedUsageFlags: " << vk::to_string(caps.supportedUsageFlags)
       << "\n";

    return os;
}

void render::create_swapchain()
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

    const auto& surface_capabilities = swapchain_details.surface_capabilities;
    log << surface_capabilities << '\n';

    uint32_t image_count = surface_capabilities.minImageCount;
    // if 0 - then limitless
    if (surface_capabilities.maxImageCount > 0u &&
        surface_capabilities.maxImageCount < image_count)
    {
        image_count = surface_capabilities.maxImageCount;
    }

    log << "image_count in swapchain minImageCount (triple buffering "
           "expected): "
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
        std::array<uint32_t, 2> indexes = { queue_indexes.graphics_family,
                                            queue_indexes.presentation_family };
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
    set_object_name(swapchain, "om_swapchain");

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
    std::ranges::transform(
        swapchain_images,
        std::back_inserter(swapchain_image_views),
        [this](vk::Image image) -> vk::ImageView
        {
            set_object_name(image, "om_swapchain_image");

            return create_image_view(
                image, swapchain_image_format, vk::ImageAspectFlagBits::eColor);
        });
    log << "create swapchain_image_views count: "
        << swapchain_image_views.size() << std::endl;
}

void render::create_renderpass()
{
    // color attachment of render pass
    vk::AttachmentDescription color_attachment{};
    color_attachment.format = swapchain_image_format;
    // number of samples to write for multisampling
    color_attachment.samples = vk::SampleCountFlagBits::e1;
    // like in OpenGL glClear() clear buffer
    // describes what to do with attachment before rendering
    color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
    // describes what to do with attachment after rendering
    color_attachment.storeOp        = vk::AttachmentStoreOp::eStore;
    color_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    // Framebuffer data will be stored as an image
    // but images can be given different data layouts
    // to give optimal use for certain operations.
    // Image data layout before renderpass starts
    color_attachment.initialLayout = vk::ImageLayout::eUndefined;
    // Image data layout after renderpass (to change to)
    color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    // Attachment reference uses an attachment index that refers to index
    // in the attachment list passed to renderpath_info
    vk::AttachmentReference attachment_ref{};
    attachment_ref.attachment = 0; // index of color_attachment in renderpath
    attachment_ref.layout     = vk::ImageLayout::eColorAttachmentOptimal;
    // ^^^ this mean that:
    // 1. starting from color_attachment.initialLayout(eUndefined)
    // 2. first conversion for subpath into (eColorAttachmentOptimal)
    // 3. final conversion after subpath to (ePresentSrcKHR)

    // Information about particular subpath the Render path is using
    vk::SubpassDescription subpass_description{};
    subpass_description.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments    = &attachment_ref;

    // Need to determine when layout transitions occur using subpass
    // dependencies
    std::array<vk::SubpassDependency, 2> subpath_dependencies{};
    // convert from eUndefined to eColorAttachmentOptimal
    auto& first_conversion = subpath_dependencies[0];
    // transition must happen after...
    // subpath index vk::SubpassExternal = special value mean outside of
    // renderpath
    first_conversion.srcSubpass = vk::SubpassExternal;
    // Pipeline stage
    first_conversion.srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    // Stage access mask (memory access)
    first_conversion.srcAccessMask = vk::AccessFlagBits::eMemoryRead;

    // transition must happen before...
    first_conversion.dstSubpass = 0; // index of first subpath
    first_conversion.dstStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    first_conversion.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                     vk::AccessFlagBits::eColorAttachmentWrite;
    first_conversion.dependencyFlags = {};

    // convert from eColorAttachmentOptimal to ePresentSrcKHR
    auto& second_conversion = subpath_dependencies[1];
    // transition must happen after...
    second_conversion.srcSubpass = 0; // index of first subpath
    // Pipeline stage
    second_conversion.srcStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    // same from previous convertion
    second_conversion.srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                      vk::AccessFlagBits::eColorAttachmentWrite;
    // transition must happen before...
    second_conversion.dstSubpass    = vk::SubpassExternal;
    second_conversion.dstStageMask  = vk::PipelineStageFlagBits::eBottomOfPipe;
    second_conversion.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    second_conversion.dependencyFlags = {};

    vk::RenderPassCreateInfo renderpath_info{};
    renderpath_info.attachmentCount = 1;
    renderpath_info.pAttachments    = &color_attachment;
    renderpath_info.subpassCount    = 1;
    renderpath_info.pSubpasses      = &subpass_description;
    renderpath_info.dependencyCount =
        static_cast<std::uint32_t>(subpath_dependencies.size());
    renderpath_info.pDependencies = subpath_dependencies.data();

    render_path = devices.logical.createRenderPass(renderpath_info);
    log << "create vulkan render path\n";

    set_object_name(render_path, "only_render_path");
}

void render::create_graphics_pipeline()
{
    // Static Pipeline States
    auto vertex_shader_code = platform_.get_file_content(
        "./02-vulkan/09-vk-res-loading-1/shaders/shader.vert.slang.spv");
    auto fragment_shader_code = platform_.get_file_content(
        "./02-vulkan/09-vk-res-loading-1/shaders/shader.frag.slang.spv");
    // compile shaders from spir-v into gpu code
    vk::ShaderModule vertex = create_shader(vertex_shader_code.as_span());
    std::experimental::scope_exit vertex_cleanup([this, &vertex]()
                                                 { destroy(vertex); });

    vk::ShaderModule fragment = create_shader(fragment_shader_code.as_span());
    std::experimental::scope_exit fragment_cleanup([this, &fragment]()
                                                   { destroy(fragment); });

    vk::PipelineShaderStageCreateInfo stage_info_vert(
        {}, vk::ShaderStageFlagBits::eVertex, vertex, "main");
    vk::PipelineShaderStageCreateInfo stage_info_frag(
        {}, vk::ShaderStageFlagBits::eFragment, fragment, "main");

    std::array<vk::PipelineShaderStageCreateInfo, 2> stages{ stage_info_vert,
                                                             stage_info_frag };
    // vertex input
    // Data for a single vertex
    vk::VertexInputBindingDescription binding_description;
    binding_description.binding = 0; // can bind multiple streams of data,
                                     // define which
    binding_description.stride = sizeof(om::vulkan::vertex);
    // how to move detween data after next vertex
    // vk::VertexInputRate::eVertex   : move to next vertex
    // vk::VertexInputRate::eInstance : move to vertex for next instance
    binding_description.inputRate = vk::VertexInputRate::eVertex;

    // how the data for an attribute is defined within a vertex
    std::array<vk::VertexInputAttributeDescription, 2> attribute_description;
    // position attribute
    // which binding the data is at (should be same as above)
    attribute_description[0].binding  = 0;
    attribute_description[0].location = 0; // location in shader where data
                                           // will be read from

    // format the data will take (also helps define size of data)
    attribute_description[0].format = vk::Format::eR32G32B32Sfloat;
    // where this attribute is defined in the data for a single vertex
    attribute_description[0].offset = offsetof(om::vulkan::vertex, pos);

    attribute_description[1].binding  = 0;
    attribute_description[1].location = 1;
    attribute_description[1].format   = vk::Format::eR32G32B32Sfloat;
    attribute_description[1].offset   = offsetof(om::vulkan::vertex, col);

    vk::PipelineVertexInputStateCreateInfo vertex_input_state_info{};
    vertex_input_state_info.vertexBindingDescriptionCount = 1;
    vertex_input_state_info.pVertexBindingDescriptions =
        &binding_description; // spacing/striding vertex info
    vertex_input_state_info.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attribute_description.size());
    vertex_input_state_info.pVertexAttributeDescriptions =
        attribute_description
            .data(); // data format where/from shader attributes

    // input assembly
    vk::PipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.topology = vk::PrimitiveTopology::eTriangleList;
    input_assembly.primitiveRestartEnable =
        vk::False; // allow override "strip" topology to start new privitives

    // viewport and scissor
    vk::Viewport viewport{};
    viewport.x        = 0.f;
    viewport.y        = 0.f;
    viewport.width    = static_cast<float>(swapchain_image_extent.width);
    viewport.height   = static_cast<float>(swapchain_image_extent.height);
    viewport.minDepth = 0.f; // min framebuffer depth
    viewport.maxDepth = 1.f; // max framebuffer depth

    vk::Rect2D scissor{};
    scissor.offset = { { 0, 0 } };           // offset to use region from
    scissor.extent = swapchain_image_extent; // region extent

    vk::PipelineViewportStateCreateInfo viewport_state_info{ {},
                                                             viewport,
                                                             scissor };
    // // Dynamic Pipeline States - we need some parts not to be backed in
    // pipeline
    // // enable dynamic states
    // // we need on every resize of Window change our pipeline viewport and
    // // scissor
    // // NOTE: remember to always to recreate swapchain images if you resize
    // // window
    // std::array<vk::DynamicState, 2> dynamic_states{
    //     vk::DynamicState::eViewport, vk::DynamicState::eScissor
    // };

    // vk::PipelineDynamicStateCreateInfo dynamic_state_info{ {}, dynamic_states
    // };

    // Rasterizer State
    vk::PipelineRasterizationStateCreateInfo rasterization_state_info{};

    // to enable -> first enable DepthClamp in
    // LogicalDeviceFreatures
    rasterization_state_info.depthClampEnable = vk::False;
    // whether to discard data and skip rasterazer
    // never creates fragments
    // only sutable for pipeline without framebuffer output
    rasterization_state_info.rasterizerDiscardEnable = vk::False;
    // how to fill points between verticles
    rasterization_state_info.polygonMode = vk::PolygonMode::eFill;
    // how thick line should be drawn (value > 1.0 should enable device
    // extension)
    rasterization_state_info.lineWidth = 1.f;
    // which face of triangle to cull
    rasterization_state_info.cullMode = vk::CullModeFlagBits::eBack;
    // which triangle face is front face
    rasterization_state_info.frontFace = vk::FrontFace::eClockwise;
    // where to add depth bias to fragments (good for stopping "shadow acne" in
    // shadow mapping)
    rasterization_state_info.depthBiasEnable = vk::False;

    // Multisampling
    vk::PipelineMultisampleStateCreateInfo multisample_state_info{};
    multisample_state_info.sampleShadingEnable = vk::False;
    // number of samples to use per fragment
    multisample_state_info.rasterizationSamples = vk::SampleCountFlagBits::e1;

    // Blending (two fragments one - current and other - framebuffer)
    //
    // blend_attachment - how blending is handled
    vk::PipelineColorBlendAttachmentState blend_attachment{};
    blend_attachment.blendEnable = vk::True;
    blend_attachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    // blending use equation:
    // (srcBlendFactor * new color) BlendOp (dstBlendFactor * old color)
    blend_attachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    blend_attachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    blend_attachment.colorBlendOp        = vk::BlendOp::eAdd;

    blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    blend_attachment.alphaBlendOp        = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo blending_state_info{};
    blending_state_info.logicOpEnable   = vk::False;
    blending_state_info.attachmentCount = 1;
    blending_state_info.pAttachments    = &blend_attachment;

    // Pipeline layout (TODO: apply future descriptor sets)
    vk::PipelineLayoutCreateInfo layout_info{};

    pipeline_layout = devices.logical.createPipelineLayout(layout_info);

    // TODO add Depth and Stensil testing

    // Graphics Pipeline creation
    vk::GraphicsPipelineCreateInfo graphics_info{};
    graphics_info.stageCount        = static_cast<std::uint32_t>(stages.size());
    graphics_info.pStages           = stages.data(); // shader stages
    graphics_info.pVertexInputState = &vertex_input_state_info;
    graphics_info.pInputAssemblyState = &input_assembly;
    graphics_info.pViewportState      = &viewport_state_info;
    graphics_info.pDynamicState       = nullptr;
    graphics_info.pRasterizationState = &rasterization_state_info;
    graphics_info.pMultisampleState   = &multisample_state_info;
    graphics_info.pColorBlendState    = &blending_state_info;
    graphics_info.pDepthStencilState  = nullptr;
    graphics_info.layout              = pipeline_layout;
    graphics_info.renderPass          = render_path;
    graphics_info.subpass             = 0;

    // Pipeline Derivatives
    // to use vulkan less memory we can create lists of pipelines
    // so we can create first as we do and all other only with changes
    // here we do not need it
    graphics_info.basePipelineHandle =
        nullptr; // existing pipeline to derive from
    graphics_info.basePipelineIndex =
        -1; // or index of pipeline to derive from(in case of creating multiple
            // at once)

    auto result =
        devices.logical.createGraphicsPipeline(nullptr, graphics_info);
    if (result.result != vk::Result::eSuccess)
    {
        throw std::runtime_error("error: failed to create vk::Pipeline");
    }

    graphics_pipeline = std::move(result.value);

    set_object_name(graphics_pipeline, "only_graphics_pipeline");
}

void render::create_framebuffers()
{
    swapchain_framebuffers.reserve(swapchain_images.size());

    auto gen_framebuffer = [&, count = 0](const vk::ImageView& view) mutable
    {
        std::array<vk::ImageView, 1> attachments{ view };

        vk::FramebufferCreateInfo info{};
        info.renderPass      = render_path; // render path layout to be used
        info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
        info.pAttachments    = attachments.data(); // 1:1 with renderpath
        info.width           = swapchain_image_extent.width;
        info.height          = swapchain_image_extent.height;
        info.layers          = 1;

        vk::Framebuffer framebuffer = devices.logical.createFramebuffer(info);
        if (!framebuffer)
        {
            throw std::runtime_error("can't create framebuffer");
        }
        set_object_name(framebuffer,
                        "om_framebuffer_" + std::to_string(count++));
        return framebuffer;
    };

    std::ranges::transform(swapchain_image_views,
                           std::back_inserter(swapchain_framebuffers),
                           gen_framebuffer);
}

void render::create_command_pool()
{
    if (!queue_indexes.is_valid())
    {
        throw std::runtime_error("error: queue indexes not valid");
    }

    log << "create command pool\n";

    vk::CommandPoolCreateInfo info{};
    info.queueFamilyIndex = queue_indexes.graphics_family;
    info.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    graphics_command_pool = devices.logical.createCommandPool(info);

    if (!graphics_command_pool)
    {
        throw std::runtime_error("error: can't create graphics command pool");
    }

    set_object_name(graphics_command_pool, "om_graphics_cmd_pool");
}

void render::create_command_buffers()
{
    log << "create command buffers count: " << swapchain_framebuffers.size()
        << "\n";

    vk::CommandBufferAllocateInfo info{};
    info.commandPool = graphics_command_pool;
    info.level       = vk::CommandBufferLevel::ePrimary;
    // vk::CommandBufferLevel::ePrimary; // can't be called from other buffers
    //                                   // only will be executed by the queue
    // vk::CommandBufferLevel::eSecondary; // Buffer can't be called directly.
    //                                     // Can be called from other command
    //                                     // using vkCmdExecuteCommands when
    //                                     // recording commands in primary
    //                                     // buffer
    info.commandBufferCount =
        static_cast<std::uint32_t>(swapchain_framebuffers.size());

    command_buffers = devices.logical.allocateCommandBuffers(info);

    if (command_buffers.empty())
    {
        throw std::runtime_error("error: can't allocate command buffers");
    }

    std::ranges::for_each(command_buffers,
                          [this, count = 0](vk::CommandBuffer& buffer) mutable
                          {
                              set_object_name(buffer,
                                              "command_buffer_per_swapchain_" +
                                                  std::to_string(count++));
                          });
}

void render::record_commands()
{
    vk::CommandBufferBeginInfo begin_info{};
    // can be submitted multiple times without waiting for previous submission
    // we do not need to use simutaneous use cause we have start using fence
    // begin_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

    vk::RenderPassBeginInfo render_pass_info{};
    render_pass_info.renderPass        = render_path;
    render_pass_info.renderArea.offset = vk::Offset2D{ 0, 0 }; // in pixels
    render_pass_info.renderArea.extent = swapchain_image_extent;

    vk::ClearValue clear_color = vk::ClearColorValue(
        std::array<float, 4>{ 0.6f, 0.65f, 0.4f, 1.f }); // RGBA

    render_pass_info.pClearValues = &clear_color; // TODO depth attachment later
    render_pass_info.clearValueCount = 1;

    auto record_commands =
        [&](std::tuple<vk::CommandBuffer&, vk::Framebuffer&> cmd_and_fb)
    {
        auto& [buffer, fb] = cmd_and_fb;
        // start recording
        buffer.begin(begin_info, dynamic_loader);
        {
            render_pass_info.framebuffer = fb;
            buffer.beginRenderPass(
                render_pass_info, vk::SubpassContents::eInline, dynamic_loader);
            {

                buffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                    graphics_pipeline,
                                    dynamic_loader);
                std::array<vk::Buffer, 1> buffers{
                    first_mesh.get_vertex_buffer()
                };
                std::array<vk::DeviceSize, 1> offsets{ 0 };

                buffer.bindVertexBuffers(0, // first binding
                                         buffers,
                                         offsets,
                                         dynamic_loader);

                buffer.draw(first_mesh.get_vertex_count(),
                            1, // 1 instance
                            0, // first vertex
                            0, // first instance
                            dynamic_loader);
            }
            buffer.endRenderPass(dynamic_loader);
        }
        // finish recording
        buffer.end(dynamic_loader);
    };

    std::ranges::for_each(
        std::views::zip(command_buffers, swapchain_framebuffers),
        record_commands);
}

void render::create_synchronization_objects()
{
    synchronization.image_available.resize(max_frames_in_gpu);
    synchronization.render_finished.resize(max_frames_in_gpu);
    synchronization.gpu_fence.resize(max_frames_in_gpu);

    struct gen_sem
    {
        render&     self;
        std::string name;
        int         index;

        vk::Semaphore operator()()
        {
            auto semaphore = self.devices.logical.createSemaphore({});
            self.set_object_name(semaphore, name + std::to_string(index++));
            return semaphore;
        }
    };

    std::ranges::generate(
        synchronization.image_available,
        gen_sem{ .self = *this, .name = "image_available_", .index = 0 });

    std::ranges::generate(
        synchronization.render_finished,
        gen_sem{ .self = *this, .name = "render_finished_", .index = 0 });

    std::ranges::generate(synchronization.gpu_fence,
                          [this, i = 0]() mutable
                          {
                              vk::FenceCreateInfo info{};
                              info.flags = vk::FenceCreateFlagBits::eSignaled;
                              auto fence = devices.logical.createFence(info);
                              set_object_name(fence,
                                              "fence_" + std::to_string(i++));
                              return fence;
                          });
}

void render::destroy_synchronization_objects() noexcept
{
    auto destroy_sem = [this](vk::Semaphore& semaphore)
    { devices.logical.destroy(semaphore); };

    std::ranges::for_each(synchronization.image_available, destroy_sem);
    std::ranges::for_each(synchronization.render_finished, destroy_sem);

    std::ranges::for_each(synchronization.gpu_fence,
                          [this](vk::Fence& fence)
                          { devices.logical.destroy(fence); });
}

vk::Extent2D render::choose_best_swapchain_image_resolution(
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

    platform_interface::buffer_size buffer_size =
        platform_.get_window_buffer_size();

    extent.width  = buffer_size.width;
    extent.height = buffer_size.height;

    log << "use extent2d from callback_get_window_buffer_size: "
        << buffer_size.width << 'x' << buffer_size.height << std::endl;

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

vk::SurfaceFormatKHR render::choose_best_surface_format(
    std::span<vk::SurfaceFormatKHR> formats)
{
    vk::SurfaceFormatKHR default_format(vk::Format::eR8G8B8A8Unorm,
                                        vk::ColorSpaceKHR::eSrgbNonlinear);

    if (formats.empty())
    {
        throw std::runtime_error("empty surface formats");
    }

    if (formats.size() == 1 && formats.front().format == vk::Format::eUndefined)
    {
        // this means all formats are supported!
        // so let's use our defaults
        return default_format;
    }
    // not all supported search for RGB or BGR
    std::array<vk::SurfaceFormatKHR, 2> suitable_formats = {
        default_format,
        { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear }
    };
    auto it = std::ranges::find_first_of(formats, std::span(suitable_formats));

    if (it != formats.end())
    {
        return *it;
    }

    // can't find suitable format lets try first as it is
    return formats.front();
}

vk::PresentModeKHR render::choose_best_present_mode(
    std::span<vk::PresentModeKHR> present_modes)
{
    if (std::ranges::contains(present_modes, vk::PresentModeKHR::eMailbox))
    {
        return vk::PresentModeKHR::eMailbox;
    }
    // guaranteed to be in any vulkan implementation
    return vk::PresentModeKHR::eFifo;
}

render::swapchain_details_t render::get_swapchain_details(
    vk::PhysicalDevice& device)
{
    swapchain_details_t details{};
    details.surface_capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.surface_formats      = device.getSurfaceFormatsKHR(surface);
    details.presentation_modes   = device.getSurfacePresentModesKHR(surface);
    return details;
}

vk::ImageView render::create_image_view(vk::Image            image,
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

    auto image_view = devices.logical.createImageView(info);

    return image_view;
}

vk::ShaderModule render::create_shader(std::span<std::byte> spir_v)
{
    log << "create shader module\n";
    vk::ShaderModuleCreateInfo create_info(
        {}, spir_v.size(), reinterpret_cast<const uint32_t*>(spir_v.data()));

    return devices.logical.createShaderModule(create_info);
}

void render::destroy_surface() noexcept
{
    platform_.destroy_vulkan_surface(instance, surface, nullptr);
    log << "vulkan surface destroyed\n";
}

void render::destroy(vk::ShaderModule& shader) noexcept
{
    log << "destroy shader module\n";
    devices.logical.destroy(shader);
}
} // namespace om::vulkan
