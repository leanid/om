#pragma once

#include <iosfwd>
#include <limits>
#include <span>
#include <string_view>
#include <vector>

#include <experimental/scope> // not found on macOS

#include <vulkan/vulkan.hpp>

namespace om::vulkan
{
struct platform_interface
{
    virtual ~platform_interface() = default;

    struct extensions
    {
        const char* const* names = nullptr;
        std::uint32_t      count = 0u;
    };
    struct buffer_size
    {
        std::uint32_t width  = 0u;
        std::uint32_t height = 0u;
    };
    struct content
    {
        std::unique_ptr<std::byte[]> memory;
        std::size_t                  size{};

        content(const content& other)            = delete;
        content& operator=(const content& other) = delete;

        content() noexcept
            : memory{}
            , size{}
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

        std::string_view as_string_view() const noexcept
        {
            return { reinterpret_cast<char*>(memory.get()), size };
        }
        std::span<std::byte> as_span() const noexcept
        {
            return std::span{ memory.get(), size };
        }
    };

    virtual extensions   get_extensions() = 0;
    virtual VkSurfaceKHR create_surface(
        VkInstance instance, VkAllocationCallbacks* alloc_callbacks) = 0;
    virtual buffer_size get_windows_buffer_size()                    = 0;

    virtual std::ostream& get_logger() = 0;

    virtual content get_file_content(std::string_view path) = 0;
};

class render
{
public:
    struct hints
    {
        bool verbose;
        bool enable_validation_layers;
    };

    explicit render(platform_interface& platform, hints hints);

    ~render();

private:
    struct swapchain_details_t
    {
        vk::SurfaceCapabilitiesKHR        surface_capabilities;
        std::vector<vk::SurfaceFormatKHR> surface_formats;
        std::vector<vk::PresentModeKHR>   presentation_modes;
    };

    void create_instance();
    void create_logical_device();
    void create_surface();
    void create_swapchain();
    void create_graphics_pipeline();

    [[nodiscard]] vk::ImageView create_image_view(
        vk::Image            image,
        vk::Format           format,
        vk::ImageAspectFlags aspect_flags) const;

    vk::ShaderModule create_shader(std::span<std::byte> spir_v);
    void             destroy(vk::ShaderModule& shader);

    void validate_expected_extensions_exists(
        const vk::InstanceCreateInfo& create_info);
    void validate_instance_layer_present(std::string_view instance_layer);
    void validate_physical_device();

    bool        check_device_suitable(vk::PhysicalDevice& physical);
    static bool check_device_extension_supported(
        vk::PhysicalDevice& device, std::string_view extension_name);

    void                get_physical_device();
    swapchain_details_t get_swapchain_details(vk::PhysicalDevice& device);

    static uint32_t get_render_queue_family_index(
        const vk::PhysicalDevice& physical_device);
    static uint32_t get_presentation_queue_family_index(
        const vk::PhysicalDevice& physical_device,
        const vk::SurfaceKHR&     surface_to_check);

    vk::Extent2D choose_best_swapchain_image_resolution(
        const vk::SurfaceCapabilitiesKHR& capabilities);
    static vk::SurfaceFormatKHR choose_best_surface_format(
        std::span<vk::SurfaceFormatKHR> formats);
    static vk::PresentModeKHR choose_best_present_mode(
        std::span<vk::PresentModeKHR> present_modes);

    friend std::ostream& operator<<(std::ostream&              os,
                                    const swapchain_details_t& details);

    // render external interface objects
    std::ostream&       log;
    platform_interface& platform_;
    hints               hints_;

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
} // namespace om::vulkan