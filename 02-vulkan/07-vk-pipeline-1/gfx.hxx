#include <functional>
#include <iostream>
#include <limits>
#include <ostream>
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

    virtual extensions   get_extensions() = 0;
    virtual VkSurfaceKHR create_surface(
        VkInstance instance, VkAllocationCallbacks* alloc_callbacks) = 0;

    struct buffer_size
    {
        std::uint32_t width  = 0u;
        std::uint32_t height = 0u;
    };

    virtual buffer_size get_windows_buffer_size() = 0;
};

class render
{
public:
    // callback functions types
    using get_extensions_t = std::function<const char* const*(uint32_t*)>;
    using create_surface_t =
        std::function<VkSurfaceKHR(VkInstance, const VkAllocationCallbacks*)>;
    using get_window_size_t =
        std::function<void(uint32_t* width, uint32_t* height)>;

    struct hints_t
    {
        bool verbose;
        bool enable_validation_layers;
    };

    explicit render(std::ostream&     log,
                    get_extensions_t  get_instance_extensions,
                    create_surface_t  create_vk_surface,
                    get_window_size_t get_window_buffer_size,
                    hints_t           hints);

    ~render();

private:
    struct swapchain_details_t
    {
        vk::SurfaceCapabilitiesKHR        surface_capabilities;
        std::vector<vk::SurfaceFormatKHR> surface_formats;
        std::vector<vk::PresentModeKHR>   presentation_modes;
    };

    void create_instance(get_extensions_t get_instance_extensions);
    void create_logical_device();
    void create_surface(const create_surface_t& create_vk_surface);
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
} // namespace om::vulkan
