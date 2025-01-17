#pragma once

#include <cstdint>
#include <iosfwd>
#include <limits>
#include <memory> // std::unique_ptr
#include <span>
#include <string_view>
#include <utility> // std::exchange
#include <vector>

#include <experimental/scope> // not found on macOS

#undef VULKAN_HPP_NO_EXCEPTIONS // exceptions are enabled by default
                                // but we want to be explicit
#define VULKAN_HPP_NAMESPACE vk // explicit namespace to help IDE
#include <vulkan/vulkan.hpp>

namespace om::vulkan
{
struct platform_interface
{
    virtual ~platform_interface() = default;

    struct extensions
    {
        std::vector<const char*> names;
    };
    struct buffer_size
    {
        std::uint32_t width  = 0u;
        std::uint32_t height = 0u;
    };
    struct content
    {
        std::unique_ptr<std::byte[]> memory; // NOLINT
        std::size_t                  size{};

        content(const content& other)            = delete;
        content& operator=(const content& other) = delete;

        content() noexcept
            : memory{}
        {
        }
        content(content&& other) noexcept
            : memory{ std::move(other.memory) }
            , size{ std::exchange(other.size, 0) }
        {
        }

        content& operator=(content&& other) noexcept
        {
            memory = std::move(other.memory);
            size   = std::exchange(other.size, 0);
            return *this;
        }

        [[nodiscard]] std::span<std::byte> as_span() const noexcept
        {
            return std::span{ memory.get(), size };
        }
    };

    virtual extensions   get_vulkan_extensions() = 0;
    virtual VkSurfaceKHR create_vulkan_surface(
        VkInstance instance, VkAllocationCallbacks* alloc_callbacks) = 0;
    virtual void destroy_vulkan_surface(
        VkInstance             instance,
        VkSurfaceKHR           surface,
        VkAllocationCallbacks* alloc_callbacks)  = 0;
    virtual buffer_size get_window_buffer_size() = 0;

    virtual std::ostream& get_logger() = 0;

    virtual content get_file_content(std::string_view path) = 0;
};

class render
{
public:
    struct hints
    {
        struct
        {
            std::uint32_t major = 1;
            std::uint32_t minor = 0;
        } vulkan_version;
        bool verbose                   = false;
        bool enable_validation_layers  = false;
        bool enable_debug_callback_ext = false;
    };

    explicit render(platform_interface& platform, hints hints);

    ~render();

    /// @brief Render the frame
    void draw();

private:
    // create functions
    void create_instance(bool enable_validation_layers,
                         bool enable_debug_callback_ext);
    void create_debug_callback(bool enable_debug_callback);
    void create_logical_device();
    void create_surface();
    void create_swapchain();
    void create_renderpass();
    void create_graphics_pipeline();
    void create_framebuffers();
    void create_command_pool();
    void create_command_buffers();
    void create_synchronization_objects();

    [[nodiscard]] vk::ImageView create_image_view(
        vk::Image            image,
        vk::Format           format,
        vk::ImageAspectFlags aspect_flags) const;

    vk::ShaderModule create_shader(std::span<std::byte> spir_v);

    // record functions
    void record_commands();

    // destroy functions
    void destroy_synchronization_objects();
    void destroy_debug_callback();
    void destroy_surface();
    void destroy(vk::ShaderModule& shader);

    // validation functions
    void validate_expected_extensions_exists(
        const vk::InstanceCreateInfo& create_info);
    void validate_instance_layer_present(std::string_view instance_layer);
    void validate_physical_device();

    bool        check_device_suitable(vk::PhysicalDevice& physical);
    static bool check_device_extension_supported(
        vk::PhysicalDevice& device, std::string_view extension_name);

    // get functions
    void get_physical_device();

    struct swapchain_details_t
    {
        vk::SurfaceCapabilitiesKHR        surface_capabilities;
        std::vector<vk::SurfaceFormatKHR> surface_formats;
        std::vector<vk::PresentModeKHR>   presentation_modes;
    };

    swapchain_details_t get_swapchain_details(vk::PhysicalDevice& device);

    static uint32_t get_render_queue_family_index(
        const vk::PhysicalDevice& physical_device);
    static uint32_t get_presentation_queue_family_index(
        const vk::PhysicalDevice& physical_device,
        const vk::SurfaceKHR&     surface_to_check);

    // choose functions
    vk::Extent2D choose_best_swapchain_image_resolution(
        const vk::SurfaceCapabilitiesKHR& capabilities);
    static vk::SurfaceFormatKHR choose_best_surface_format(
        std::span<vk::SurfaceFormatKHR> formats);
    static vk::PresentModeKHR choose_best_present_mode(
        std::span<vk::PresentModeKHR> present_modes);

    friend std::ostream& operator<<(std::ostream&              os,
                                    const swapchain_details_t& details);
    // debug functions
    void set_object_name(auto object, const std::string& name)
    {
        if (!hints_.enable_debug_callback_ext)
        {
            return;
        }

        vk::DebugUtilsObjectNameInfoEXT name_info;
        name_info.objectType   = object.objectType;
        name_info.objectHandle = reinterpret_cast<uint64_t>(
            static_cast<decltype(object)::NativeType>(object));
        name_info.pObjectName = name.c_str();
        devices.logical.setDebugUtilsObjectNameEXT(name_info, dynamic_loader);
    }

    // render external interface objects
    std::ostream&       log;
    platform_interface& platform_;
    hints               hints_;

    // vulkan main objects
    vk::Instance instance;
    // dynamic loader is used to load vulkan functions for extensions
    vk::detail::DispatchLoaderDynamic dynamic_loader;
    // debug extension is not available on macOS
    vk::DebugUtilsMessengerEXT debug_extension;

    // mod(max_frames_in_gpu) used to avoid blocking the CPU
    uint32_t current_frame_index = 0;

    struct
    {
        vk::PhysicalDevice physical;
        vk::Device         logical;
    } devices;

    [[maybe_unused]] vk::Queue     render_queue;
    [[maybe_unused]] vk::Queue     presentation_queue;
    vk::SurfaceKHR                 surface; // KHR - extension
    vk::SwapchainKHR               swapchain;
    std::vector<vk::Image>         swapchain_images;
    std::vector<vk::ImageView>     swapchain_image_views;
    std::vector<vk::Framebuffer>   swapchain_framebuffers;
    std::vector<vk::CommandBuffer> command_buffers;

    // vulkan pipeline
    vk::Pipeline       graphics_pipeline{};
    vk::PipelineLayout pipeline_layout{};
    vk::RenderPass     render_path{};

    // pools
    vk::CommandPool graphics_command_pool{};

    // vulkan utilities
    vk::Format   swapchain_image_format{};
    vk::Extent2D swapchain_image_extent{};

    // sinhronization
    struct
    {
        std::vector<vk::Semaphore> image_available{}; // GPU to GPU only sync
        std::vector<vk::Semaphore> render_finished{}; // GPU to CPU sync
        std::vector<vk::Fence>     gpu_fence{};
    } synchronization;

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
} // namespace om::vulkan
