module;

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

#include "experimental/report_duration.hxx"
#include "experimental/scope"

export module vulkan_render;

import log;
import std;
import glm;
import vulkan_hpp;

namespace om::vulkan
{
export struct vertex final
{
    glm::vec3 pos; // vertex positions x, y, z
    glm::vec3 col; // vertex color r, g, b
};
export class render;
export class mesh final
{
public:
    mesh() = default;
    mesh(vk::PhysicalDevice physical_device,
         vk::Device         device,
         std::span<vertex>  vertexes,
         render&            render);
    mesh(const mesh& other)            = delete;
    mesh& operator=(const mesh& other) = delete;
    mesh(mesh&& other);
    mesh& operator=(mesh&& other);
    ~mesh();

    [[nodiscard]] uint32_t get_vertex_count() const;
    vk::Buffer             get_vertex_buffer();

    void cleanup() noexcept;

private:
    void     create_buffer(std::span<vertex> vertexes, render& render);
    uint32_t find_mem_type_index(uint32_t                allowed_types,
                                 vk::MemoryPropertyFlags properties);

    vk::PhysicalDevice physical_device;
    vk::Device         device;
    vk::Buffer         buffer;
    vk::DeviceMemory   vertex_buf_mem;
    uint32_t           num_vertexes{};
};

export struct platform_interface
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
    }; // struct content

    virtual extensions     get_vulkan_extensions() = 0;
    virtual vk::SurfaceKHR create_vulkan_surface(
        vk::Instance instance, vk::AllocationCallbacks* alloc_callbacks) = 0;
    virtual void destroy_vulkan_surface(
        vk::Instance             instance,
        vk::SurfaceKHR           surface,
        vk::AllocationCallbacks* alloc_callbacks) noexcept = 0;
    virtual buffer_size get_window_buffer_size()           = 0;

    virtual std::ostream& get_logger() noexcept = 0;

    virtual content get_file_content(std::string_view path) = 0;
};

export class render
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
        devices.logical.setDebugUtilsObjectNameEXT(name_info);
    }

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
    void destroy_synchronization_objects() noexcept;
    void destroy_debug_callback() noexcept;
    void destroy_surface() noexcept;
    void destroy(vk::ShaderModule& shader) noexcept;

    // validation functions
    void validate_expected_extensions_exists(
        const vk::InstanceCreateInfo& create_info);
    void validate_instance_layers_present(
        std::vector<std::string_view> required_layers);
    void validate_physical_device();

    bool        check_device_suitable(const vk::PhysicalDevice& physical);
    static bool check_device_extension_supported(
        const vk::PhysicalDevice& device, std::string_view extension_name);

    // get functions
    void get_physical_device();

    struct swapchain_details_t
    {
        vk::SurfaceCapabilitiesKHR        surface_capabilities;
        std::vector<vk::SurfaceFormatKHR> surface_formats;
        std::vector<vk::PresentModeKHR>   presentation_modes;
    };

    swapchain_details_t get_swapchain_details(const vk::PhysicalDevice& device);

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

    // render external interface objects
    std::ostream&       log;
    platform_interface& platform;
    hints               hints_;

    // main vulkan objects
    vk::raii::Context                context;
    vk::raii::Instance               instance        = nullptr;
    vk::raii::DebugUtilsMessengerEXT debug_messenger = nullptr;
    vk::raii::SurfaceKHR             surface = nullptr; // KHR - extension

    struct
    {
        vk::raii::PhysicalDevice physical = nullptr;
        vk::raii::Device         logical  = nullptr;
    } devices;

    vk::raii::Queue graphics_queue     = nullptr; // can be presentation too
    vk::raii::Queue presentation_queue = nullptr; // only if needed

    struct
    {
        union
        {
            struct
            {
                std::uint32_t graphics     = ~0u;
                std::uint32_t presentation = ~0u;
            } index;
            std::array<std::uint32_t, 2> array;
        };
    } queue_family;

    static_assert(sizeof(decltype(queue_family.array)) ==
                  sizeof(queue_family.index));


    // vulkan main objects
    // vk::Instance instance;
    // dynamic loader is used to load vulkan functions for extensions
    vk::detail::DispatchLoaderDynamic dynamic_loader;
    // debug extension is not available on macOS
    // vk::DebugUtilsMessengerEXT debug_extension;

    // mod(max_frames_in_gpu) used to avoid blocking the CPU
    uint32_t current_frame_index = 0;

    vk::raii::SwapchainKHR         swapchain = nullptr;
    std::vector<vk::Image>         swapchain_images;
    std::vector<vk::ImageView>     swapchain_image_views;
    std::vector<vk::Framebuffer>   swapchain_framebuffers;
    std::vector<vk::CommandBuffer> command_buffers;

    // vulkan pipeline
    vk::raii::Pipeline       graphics_pipeline = nullptr;
    vk::raii::PipelineLayout pipeline_layout   = nullptr;
    vk::raii::RenderPass     render_path       = nullptr;

    // pools
    vk::CommandPool graphics_command_pool{};

    // vulkan utilities
    vk::Format   swapchain_image_format{};
    vk::Extent2D swapchain_image_extent{};

    // sinchronization
    struct
    {
        std::vector<vk::Semaphore> image_available{}; // GPU to GPU only sync
        std::vector<vk::Semaphore> render_finished{}; // GPU to CPU sync
        std::vector<vk::Fence>     gpu_fence{};
    } synchronization;

    const std::vector<const char*> required_device_extensions{
        vk::KHRSwapchainExtensionName,
        vk::KHRSpirv14ExtensionName,
        vk::KHRSynchronization2ExtensionName,
        vk::KHRCreateRenderpass2ExtensionName
    };

    // scene component objects
    mesh first_mesh;
};

mesh::mesh(vk::PhysicalDevice physical_device,
           vk::Device         device,
           std::span<vertex>  vertexes,
           render&            render)
    : physical_device(physical_device)
    , device(device)
    , buffer()
    , vertex_buf_mem()
    , num_vertexes(vertexes.size())
{
    create_buffer(vertexes, render);
}

uint32_t mesh::get_vertex_count() const
{
    return num_vertexes;
}

vk::Buffer mesh::get_vertex_buffer()
{
    return buffer;
}
void mesh::create_buffer(std::span<vertex> vertexes, render& render)
{
    // information to create buffer (doesn't include assining memory)
    vk::BufferCreateInfo info;
    info.size        = sizeof(vertex) * vertexes.size();
    info.usage       = vk::BufferUsageFlagBits::eVertexBuffer;
    info.sharingMode = vk::SharingMode::eExclusive; // similar to swapchain
                                                    // images, can share vertex
                                                    // buffers

    buffer = device.createBuffer(info);
    render.set_object_name(buffer, "first_vertex_buffer");
    // get buffer memory requirements
    vk::MemoryRequirements mem_requirements =
        device.getBufferMemoryRequirements(buffer);

    // allocate memory to buffer
    vk::MemoryAllocateInfo mem_alloc_info;
    mem_alloc_info.allocationSize = mem_requirements.size;
    mem_alloc_info.memoryTypeIndex =
        find_mem_type_index(mem_requirements.memoryTypeBits,
                            vk::MemoryPropertyFlagBits::eHostVisible |
                                vk::MemoryPropertyFlagBits::eHostCoherent);

    // allocate memory to vk::device
    vertex_buf_mem = device.allocateMemory(mem_alloc_info);
    if (!vertex_buf_mem)
    {
        throw std::runtime_error("can't allocate vertex buf memory");
    }
    render.set_object_name(vertex_buf_mem, "first_vertex_buff_memory");

    // bind buffer and memory
    device.bindBufferMemory(buffer, vertex_buf_mem, 0 //< memory offset
    );

    // map memory to vertex buffer
    void* mem = device.mapMemory(vertex_buf_mem, 0, info.size);
    std::uninitialized_copy_n(
        vertexes.begin(), vertexes.size(), static_cast<vertex*>(mem));
    device.unmapMemory(vertex_buf_mem);
}

uint32_t mesh::find_mem_type_index(uint32_t                allowed_types,
                                   vk::MemoryPropertyFlags properties)
{
    // get properties of physical device memory
    vk::PhysicalDeviceMemoryProperties physical_mem_prop =
        physical_device.getMemoryProperties();

    for (uint32_t i = 0; i < physical_mem_prop.memoryTypeCount; ++i)
    {
        if (!(allowed_types & (1 << i)))
        {
            continue;
        }
        auto phys_mem_flags = physical_mem_prop.memoryTypes[i].propertyFlags;
        if ((phys_mem_flags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error(
        "can't find memory of allowed_types and properties");
}
void mesh::cleanup() noexcept
{
    using om::cout;
    if (!num_vertexes)
    {
        return;
    }
    cout << "mesh::cleanup" << std::endl;
    if (buffer)
    {
        cout << "destroy mesh buffer" << std::endl;
        device.destroyBuffer(buffer);
    }
    if (vertex_buf_mem)
    {
        cout << "destroy mesh vertex_buf_mem" << std::endl;
        device.freeMemory(vertex_buf_mem);
    }
    num_vertexes = 0;
}
mesh::mesh(mesh&& other)
    : physical_device(other.physical_device)
    , device(other.device)
    , buffer(std::exchange(other.buffer, vk::Buffer()))
    , vertex_buf_mem(std::exchange(other.vertex_buf_mem, vk::DeviceMemory()))
    , num_vertexes(std::exchange(other.num_vertexes, 0))
{
}
mesh& mesh::operator=(mesh&& other)
{
    physical_device = other.physical_device;
    device          = other.device;
    buffer          = std::exchange(other.buffer, vk::Buffer());
    vertex_buf_mem  = std::exchange(other.vertex_buf_mem, vk::DeviceMemory());
    num_vertexes    = std::exchange(other.num_vertexes, 0);
    return *this;
}

mesh::~mesh()
{
    cleanup();
}
} // namespace om::vulkan

namespace om::vulkan
{
/// @breaf maximum number of frames to be processed simultaneously by the GPU
// static constexpr size_t max_frames_in_gpu = 3;

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
static /* VKAPI_ATTR */ std::uint32_t /* VKAPI_CALL */
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

    return vk::False; // VK_FALSE;
}

static std::string api_version_to_string(uint32_t apiVersion)
{
    std::stringstream version;
    version << vk::versionMajor(apiVersion) << '.'
            << vk::versionMinor(apiVersion) << '.'
            << vk::versionPatch(apiVersion);
    return version.str();
}

render::render(platform_interface& platform, hints hints)
    : log{ platform.get_logger() }
    , platform{ platform }
    , hints_{ hints }
    , queue_family{}
{
    create_instance(hints.enable_validation_layers,
                    hints.enable_debug_callback_ext);
    create_debug_callback(hints.enable_debug_callback_ext);
    create_surface();
    get_physical_device();
    validate_physical_device();
    create_logical_device();
    // add debug names to vk objects
    set_object_name(*instance, "om_main_instance");
    set_object_name(*surface, "om_main_surface");
    set_object_name(*devices.physical, "om_physical_device");
    set_object_name(*devices.logical, "om_logical_device");

    create_swapchain();
    create_renderpass();
    create_graphics_pipeline();
    create_framebuffers();
    create_command_pool();
    create_command_buffers();
    record_commands();
    create_synchronization_objects();

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
        (*devices.logical)
            .freeCommandBuffers(graphics_command_pool, command_buffers);
        log << "vulkan command buffers freed\n";
        (*devices.logical).destroyCommandPool(graphics_command_pool);
        log << "vulkan destroy command pool\n";
        std::ranges::for_each(
            swapchain_framebuffers,
            [this](vk::Framebuffer& framebuffer)
            { (*devices.logical).destroyFramebuffer(framebuffer); });
        log << "vulkan framebuffers destroyed\n";
        (*devices.logical).destroy(graphics_pipeline);
        log << "vulkan graphics_pipeline destroyed\n";
        (*devices.logical).destroy(pipeline_layout);
        log << "vulkan pipeline_leyout destroyed\n";
        (*devices.logical).destroy(render_path);
        log << "vulkan render_path destroyed\n";
        std::ranges::for_each(
            swapchain_image_views,
            [this](vk::ImageView image_view)
            { (*devices.logical).destroyImageView(image_view); });
        log << "vulkan swapchain image views destroyed\n";
        (*devices.logical).destroy(swapchain);
        log << "vulkan swapchain destroyed\n";
        (*devices.logical).destroy();
        log << "vulkan logical device destroyed\n";
        destroy_surface();

        destroy_debug_callback();
        log << "vulkan debug callback destroyed\n";
        instance.clear();
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
        auto fence_op_result =
            (*devices.logical)
                .waitForFences(
                    1, // fenceCount
                    &synchronization.gpu_fence.at(current_frame_index),
                    vk::True,                            // waitAll
                    std::numeric_limits<uint64_t>::max() // timeout
                );

        if (fence_op_result != vk::Result::eSuccess)
        {
            throw std::runtime_error("error: can't wait for fence");
        }

        fence_op_result = (*devices.logical)
                              .resetFences(1, // fenceCount
                                           &synchronization.gpu_fence.at(
                                               current_frame_index));

        if (fence_op_result != vk::Result::eSuccess)
        {
            throw std::runtime_error("error: can't reset fence");
        }

        auto image_index =
            (*devices.logical)
                .acquireNextImageKHR(
                    swapchain,
                    std::numeric_limits<uint64_t>::max(), // timeout
                    synchronization.image_available.at(current_frame_index),
                    {} // fence
                );

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

        auto& cmd_buffer = command_buffers.at(
            index_from_swapchain); // index_from_swapchain current_frame_index

        vk::SubmitInfo submit_info{};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores =
            &synchronization.image_available.at(index_from_swapchain);
        submit_info.pWaitDstStageMask    = &wait_stages;
        submit_info.commandBufferCount   = 1;
        submit_info.pCommandBuffers      = &cmd_buffer;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores =
            &synchronization.render_finished.at(index_from_swapchain);

        graphics_queue.submit(
            submit_info,
            // set fence to vk::True after queue finish execution
            synchronization.gpu_fence.at(index_from_swapchain));
    }

    // Present image to screen
    {
        vk::PresentInfoKHR present_info{};
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores =
            &synchronization.render_finished.at(index_from_swapchain);
        present_info.swapchainCount = 1;
        present_info.pSwapchains    = &(*swapchain);
        present_info.pImageIndices  = &index_from_swapchain;

        auto result = graphics_queue.presentKHR(present_info);

        if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error("error: can't present image to screen");
        }
    }

    current_frame_index =
        (current_frame_index + 1) %
        synchronization.image_available.size(); // max_frames_in_gpu;
}

void render::create_instance(bool enable_validation_layers,
                             bool enable_debug_callback_ext)
{
    vk::ApplicationInfo application_info{
        .pApplicationName   = "om vulkan tutorial",
        .applicationVersion = vk::makeApiVersion(0u, 0u, 1u, 0u),
        .pEngineName        = "om",
        .engineVersion      = vk::makeApiVersion(0, 0, 1, 0),
        .apiVersion         = vk::makeApiVersion(
            0u, hints_.vulkan_version.major, hints_.vulkan_version.minor, 0u)
    };

    vk::InstanceCreateInfo instance_create_info;
    instance_create_info.pApplicationInfo = &application_info;

    platform_interface::extensions extensions =
        platform.get_vulkan_extensions();

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
        extensions.names.push_back(vk::EXTDebugUtilsExtensionName);
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
            validate_instance_layers_present({ layer });

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

    instance = vk::raii::Instance(context, instance_create_info);

    log << "vulkan instance created\n";
    log << "vk api version: " << hints_.vulkan_version.major << '.'
        << hints_.vulkan_version.minor << " requested\n";

    log << "vk api version: " << vk::versionMajor(application_info.apiVersion)
        << '.' << vk::versionMinor(application_info.apiVersion)
        << " after instance created\n";

    uint32_t maximum_supported = vk::enumerateInstanceVersion();
    log << "vk api version: " << vk::versionMajor(maximum_supported) << '.'
        << vk::versionMinor(maximum_supported) << '.'
        << vk::versionPatch(maximum_supported) << " maximum supported\n";

    // vk::DispatchLoaderDynamic dispatch;
    vk::detail::DynamicLoader dl;
    auto                      ptr =
        dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    dynamic_loader = vk::detail::DispatchLoaderDynamic{ *instance, ptr };
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

    debug_messenger = instance.createDebugUtilsMessengerEXT(debug_info);

    log << "vulkan debug callback created\n";
}

void render::destroy_debug_callback() noexcept
{
    debug_messenger.clear();
    log << "vulkan debug callback destroyed\n";
}

void render::validate_expected_extensions_exists(
    const vk::InstanceCreateInfo& create_info)
{
    std::vector<vk::ExtensionProperties> extension_properties =
        context.enumerateInstanceExtensionProperties();

    log << "vulkan instance extension on this machine: ["
        << extension_properties.size() << "]\n";

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

void render::validate_instance_layers_present(
    std::vector<std::string_view> required_layers)
{
    std::vector<vk::LayerProperties> available_layers =
        context.enumerateInstanceLayerProperties();

    log << "all vulkan layers count [" << available_layers.size() << "]\n";
    log << "spec-version | impl-version | name and description\n";
    std::ranges::for_each(available_layers,
                          [this](const vk::LayerProperties& layer)
                          {
                              log << api_version_to_string(layer.specVersion)
                                  << ' ' << layer.implementationVersion << ' '
                                  << layer.layerName << " " << layer.description
                                  << '\n';
                          });
    for (auto const& required_layer : required_layers)
    {
        if (std::ranges::none_of(
                available_layers,
                [required_layer](auto const& layer_property)
                { return layer_property.layerName.data() == required_layer; }))
        {
            log << "see: support/vulkan/install.md how to install validation "
                   "layer\n";
            throw std::runtime_error("required layer not supported: " +
                                     std::string(required_layer));
        }
    }
}

static bool check_render_queue(const vk::QueueFamilyProperties& property)
{
    return !!(property.queueFlags & vk::QueueFlagBits::eGraphics);
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

bool render::check_device_suitable(const vk::PhysicalDevice& physical)
{
    std::vector<vk::QueueFamilyProperties> queue_properties =
        physical.getQueueFamilyProperties();

    bool supports_graphics =
        find_render_queue(queue_properties) != queue_properties.end();

    auto available_extensions = physical.enumerateDeviceExtensionProperties();
    bool supports_all_required_extensions = std::ranges::all_of(
        required_device_extensions,
        [&](std::string_view required_extension)
        {
            return std::ranges::any_of(
                available_extensions,
                [&](const auto& available_extension)
                {
                    return available_extension.extensionName.data() ==
                           required_extension;
                });
        });
    bool supports_vulkan_1_3 =
        physical.getProperties().apiVersion >= vk::ApiVersion13;

    auto features = physical.template getFeatures2<
        vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool supports_required_features =
        features.template get<vk::PhysicalDeviceVulkan13Features>()
            .dynamicRendering &&
        features
            .template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
            .extendedDynamicState;

    log << "physical_device [" << physical.getProperties().deviceName
        << "] suitable:\n"
        << "    supports_graphics: " << supports_graphics << '\n'
        << "    supports_all_required_extensions: "
        << supports_all_required_extensions << '\n'
        << "    supports_vulkan_1_3: " << supports_vulkan_1_3 << '\n'
        << "    supports_required_features: " << supports_required_features
        << '\n';

    return supports_graphics && supports_all_required_extensions &&
           supports_vulkan_1_3 && supports_required_features;
}

bool render::check_device_extension_supported(const vk::PhysicalDevice& device,
                                              std::string_view extension_name)
{
    auto extensions = device.enumerateDeviceExtensionProperties();
    auto it         = std::ranges::find_if(
        extensions,
        [&extension_name](const auto& extension)
        { return extension.extensionName.data() == extension_name; });
    return it != extensions.end();
}

inline std::ostream& operator<<(std::ostream&                       os,
                                const vk::PhysicalDeviceProperties& props)
{
    const auto& limits = props.limits;

    os << "apiVersion: " << vk::versionMajor(props.apiVersion) << "."
       << vk::versionMinor(props.apiVersion) << "."
       << vk::versionPatch(props.apiVersion) << "\n";
    os << "driverVersion: " << props.driverVersion << "\n";
    os << "vendorID: " << props.vendorID << "\n";
    os << "deviceID: " << props.deviceID << "\n";
    os << "deviceType: " << vk::to_string(props.deviceType) << "\n";
    os << "deviceName: \"" << props.deviceName << "\"\n";
    os << "pipelineCacheUUID: " << std::hex;
    for (auto byte : props.pipelineCacheUUID)
    {
        os << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    os << std::dec << "\n";

    os << "limits:\n";
    os << "  maxImageDimension1D: " << limits.maxImageDimension1D << "\n";
    os << "  maxImageDimension2D: " << limits.maxImageDimension2D << "\n";
    os << "  maxImageDimension3D: " << limits.maxImageDimension3D << "\n";
    os << "  maxImageDimensionCube: " << limits.maxImageDimensionCube << "\n";
    os << "  maxImageArrayLayers: " << limits.maxImageArrayLayers << "\n";
    os << "  maxTexelBufferElements: " << limits.maxTexelBufferElements << "\n";
    os << "  maxUniformBufferRange: " << limits.maxUniformBufferRange << "\n";
    os << "  maxStorageBufferRange: " << limits.maxStorageBufferRange << "\n";
    os << "  maxPushConstantsSize: " << limits.maxPushConstantsSize << "\n";
    os << "  maxMemoryAllocationCount: " << limits.maxMemoryAllocationCount
       << "\n";
    os << "  maxSamplerAllocationCount: " << limits.maxSamplerAllocationCount
       << "\n";
    os << "  bufferImageGranularity: " << limits.bufferImageGranularity << "\n";
    os << "  sparseAddressSpaceSize: " << limits.sparseAddressSpaceSize << "\n";
    os << "  maxBoundDescriptorSets: " << limits.maxBoundDescriptorSets << "\n";
    os << "  maxPerStageDescriptorSamplers: "
       << limits.maxPerStageDescriptorSamplers << "\n";
    os << "  maxPerStageDescriptorUniformBuffers: "
       << limits.maxPerStageDescriptorUniformBuffers << "\n";
    os << "  maxPerStageDescriptorStorageBuffers: "
       << limits.maxPerStageDescriptorStorageBuffers << "\n";
    os << "  maxPerStageDescriptorSampledImages: "
       << limits.maxPerStageDescriptorSampledImages << "\n";
    os << "  maxPerStageDescriptorStorageImages: "
       << limits.maxPerStageDescriptorStorageImages << "\n";
    os << "  maxPerStageDescriptorInputAttachments: "
       << limits.maxPerStageDescriptorInputAttachments << "\n";
    os << "  maxPerStageResources: " << limits.maxPerStageResources << "\n";
    os << "  maxDescriptorSetSamplers: " << limits.maxDescriptorSetSamplers
       << "\n";
    os << "  maxDescriptorSetUniformBuffers: "
       << limits.maxDescriptorSetUniformBuffers << "\n";
    os << "  maxDescriptorSetUniformBuffersDynamic: "
       << limits.maxDescriptorSetUniformBuffersDynamic << "\n";
    os << "  maxDescriptorSetStorageBuffers: "
       << limits.maxDescriptorSetStorageBuffers << "\n";
    os << "  maxDescriptorSetStorageBuffersDynamic: "
       << limits.maxDescriptorSetStorageBuffersDynamic << "\n";
    os << "  maxDescriptorSetSampledImages: "
       << limits.maxDescriptorSetSampledImages << "\n";
    os << "  maxDescriptorSetStorageImages: "
       << limits.maxDescriptorSetStorageImages << "\n";
    os << "  maxDescriptorSetInputAttachments: "
       << limits.maxDescriptorSetInputAttachments << "\n";
    os << "  maxVertexInputAttributes: " << limits.maxVertexInputAttributes
       << "\n";
    os << "  maxVertexInputBindings: " << limits.maxVertexInputBindings << "\n";
    os << "  maxVertexInputAttributeOffset: "
       << limits.maxVertexInputAttributeOffset << "\n";
    os << "  maxVertexInputBindingStride: "
       << limits.maxVertexInputBindingStride << "\n";
    os << "  maxVertexOutputComponents: " << limits.maxVertexOutputComponents
       << "\n";
    os << "  maxTessellationGenerationLevel: "
       << limits.maxTessellationGenerationLevel << "\n";
    os << "  maxTessellationPatchSize: " << limits.maxTessellationPatchSize
       << "\n";
    os << "  maxTessellationControlPerVertexInputComponents: "
       << limits.maxTessellationControlPerVertexInputComponents << "\n";
    os << "  maxTessellationControlPerVertexOutputComponents: "
       << limits.maxTessellationControlPerVertexOutputComponents << "\n";
    os << "  maxTessellationControlPerPatchOutputComponents: "
       << limits.maxTessellationControlPerPatchOutputComponents << "\n";
    os << "  maxTessellationControlTotalOutputComponents: "
       << limits.maxTessellationControlTotalOutputComponents << "\n";
    os << "  maxTessellationEvaluationInputComponents: "
       << limits.maxTessellationEvaluationInputComponents << "\n";
    os << "  maxTessellationEvaluationOutputComponents: "
       << limits.maxTessellationEvaluationOutputComponents << "\n";
    os << "  maxGeometryShaderInvocations: "
       << limits.maxGeometryShaderInvocations << "\n";
    os << "  maxGeometryInputComponents: " << limits.maxGeometryInputComponents
       << "\n";
    os << "  maxGeometryOutputComponents: "
       << limits.maxGeometryOutputComponents << "\n";
    os << "  maxGeometryOutputVertices: " << limits.maxGeometryOutputVertices
       << "\n";
    os << "  maxGeometryTotalOutputComponents: "
       << limits.maxGeometryTotalOutputComponents << "\n";
    os << "  maxFragmentInputComponents: " << limits.maxFragmentInputComponents
       << "\n";
    os << "  maxFragmentOutputAttachments: "
       << limits.maxFragmentOutputAttachments << "\n";
    os << "  maxFragmentDualSrcAttachments: "
       << limits.maxFragmentDualSrcAttachments << "\n";
    os << "  maxFragmentCombinedOutputResources: "
       << limits.maxFragmentCombinedOutputResources << "\n";
    os << "  maxComputeSharedMemorySize: " << limits.maxComputeSharedMemorySize
       << "\n";
    os << "  maxComputeWorkGroupCount: [" << limits.maxComputeWorkGroupCount[0]
       << ", " << limits.maxComputeWorkGroupCount[1] << ", "
       << limits.maxComputeWorkGroupCount[2] << "]\n";
    os << "  maxComputeWorkGroupInvocations: "
       << limits.maxComputeWorkGroupInvocations << "\n";
    os << "  maxComputeWorkGroupSize: [" << limits.maxComputeWorkGroupSize[0]
       << ", " << limits.maxComputeWorkGroupSize[1] << ", "
       << limits.maxComputeWorkGroupSize[2] << "]\n";
    os << "  subPixelPrecisionBits: " << limits.subPixelPrecisionBits << "\n";
    os << "  subTexelPrecisionBits: " << limits.subTexelPrecisionBits << "\n";
    os << "  mipmapPrecisionBits: " << limits.mipmapPrecisionBits << "\n";
    os << "  maxDrawIndexedIndexValue: " << limits.maxDrawIndexedIndexValue
       << "\n";
    os << "  maxDrawIndirectCount: " << limits.maxDrawIndirectCount << "\n";
    os << "  maxSamplerLodBias: " << limits.maxSamplerLodBias << "\n";
    os << "  maxSamplerAnisotropy: " << limits.maxSamplerAnisotropy << "\n";
    os << "  maxViewports: " << limits.maxViewports << "\n";
    os << "  maxViewportDimensions: [" << limits.maxViewportDimensions[0]
       << ", " << limits.maxViewportDimensions[1] << "]\n";
    os << "  viewportBoundsRange: [" << limits.viewportBoundsRange[0] << ", "
       << limits.viewportBoundsRange[1] << "]\n";
    os << "  viewportSubPixelBits: " << limits.viewportSubPixelBits << "\n";
    os << "  minMemoryMapAlignment: " << limits.minMemoryMapAlignment << "\n";
    os << "  minTexelBufferOffsetAlignment: "
       << limits.minTexelBufferOffsetAlignment << "\n";
    os << "  minUniformBufferOffsetAlignment: "
       << limits.minUniformBufferOffsetAlignment << "\n";
    os << "  minStorageBufferOffsetAlignment: "
       << limits.minStorageBufferOffsetAlignment << "\n";
    os << "  minTexelOffset: " << limits.minTexelOffset << "\n";
    os << "  maxTexelOffset: " << limits.maxTexelOffset << "\n";
    os << "  minTexelGatherOffset: " << limits.minTexelGatherOffset << "\n";
    os << "  maxTexelGatherOffset: " << limits.maxTexelGatherOffset << "\n";
    os << "  minInterpolationOffset: " << limits.minInterpolationOffset << "\n";
    os << "  maxInterpolationOffset: " << limits.maxInterpolationOffset << "\n";
    os << "  subPixelInterpolationOffsetBits: "
       << limits.subPixelInterpolationOffsetBits << "\n";
    os << "  maxFramebufferWidth: " << limits.maxFramebufferWidth << "\n";
    os << "  maxFramebufferHeight: " << limits.maxFramebufferHeight << "\n";
    os << "  maxFramebufferLayers: " << limits.maxFramebufferLayers << "\n";
    os << "  framebufferColorSampleCounts: "
       << vk::to_string(limits.framebufferColorSampleCounts) << "\n";
    os << "  framebufferDepthSampleCounts: "
       << vk::to_string(limits.framebufferDepthSampleCounts) << "\n";
    os << "  framebufferStencilSampleCounts: "
       << vk::to_string(limits.framebufferStencilSampleCounts) << "\n";
    os << "  framebufferNoAttachmentsSampleCounts: "
       << vk::to_string(limits.framebufferNoAttachmentsSampleCounts) << "\n";
    os << "  maxColorAttachments: " << limits.maxColorAttachments << "\n";
    os << "  sampledImageColorSampleCounts: "
       << vk::to_string(limits.sampledImageColorSampleCounts) << "\n";
    os << "  sampledImageIntegerSampleCounts: "
       << vk::to_string(limits.sampledImageIntegerSampleCounts) << "\n";
    os << "  sampledImageDepthSampleCounts: "
       << vk::to_string(limits.sampledImageDepthSampleCounts) << "\n";
    os << "  sampledImageStencilSampleCounts: "
       << vk::to_string(limits.sampledImageStencilSampleCounts) << "\n";
    os << "  storageImageSampleCounts: "
       << vk::to_string(limits.storageImageSampleCounts) << "\n";
    os << "  maxSampleMaskWords: " << limits.maxSampleMaskWords << "\n";
    os << "  timestampComputeAndGraphics: " << std::boolalpha
       << limits.timestampComputeAndGraphics << "\n";
    os << "  timestampPeriod: " << limits.timestampPeriod << "\n";
    os << "  maxClipDistances: " << limits.maxClipDistances << "\n";
    os << "  maxCullDistances: " << limits.maxCullDistances << "\n";
    os << "  maxCombinedClipAndCullDistances: "
       << limits.maxCombinedClipAndCullDistances << "\n";
    os << "  discreteQueuePriorities: " << limits.discreteQueuePriorities
       << "\n";
    os << "  pointSizeRange: [" << limits.pointSizeRange[0] << ", "
       << limits.pointSizeRange[1] << "]\n";
    os << "  lineWidthRange: [" << limits.lineWidthRange[0] << ", "
       << limits.lineWidthRange[1] << "]\n";
    os << "  pointSizeGranularity: " << limits.pointSizeGranularity << "\n";
    os << "  lineWidthGranularity: " << limits.lineWidthGranularity << "\n";
    os << "  strictLines: " << std::boolalpha << limits.strictLines << "\n";
    os << "  standardSampleLocations: " << std::boolalpha
       << limits.standardSampleLocations << "\n";
    os << "  optimalBufferCopyOffsetAlignment: "
       << limits.optimalBufferCopyOffsetAlignment << "\n";
    os << "  optimalBufferCopyRowPitchAlignment: "
       << limits.optimalBufferCopyRowPitchAlignment << "\n";
    os << "  nonCoherentAtomSize: " << limits.nonCoherentAtomSize << "\n";

    os << "sparseProperties:\n";
    os << "  residencyStandard2DBlockShape: " << std::boolalpha
       << props.sparseProperties.residencyStandard2DBlockShape << "\n";
    os << "  residencyStandard2DMultisampleBlockShape: " << std::boolalpha
       << props.sparseProperties.residencyStandard2DMultisampleBlockShape
       << "\n";
    os << "  residencyStandard3DBlockShape: " << std::boolalpha
       << props.sparseProperties.residencyStandard3DBlockShape << "\n";
    os << "  residencyAlignedMipSize: " << std::boolalpha
       << props.sparseProperties.residencyAlignedMipSize << "\n";
    os << "  residencyNonResidentStrict: " << std::boolalpha
       << props.sparseProperties.residencyNonResidentStrict << "\n";

    return os;
}

inline std::ostream& operator<<(std::ostream&                     os,
                                const vk::PhysicalDeviceFeatures& features)
{
    os << std::boolalpha; // Print booleans as true/false

    os << "robustBufferAccess: " << features.robustBufferAccess << "\n";
    os << "fullDrawIndexUint32: " << features.fullDrawIndexUint32 << "\n";
    os << "imageCubeArray: " << features.imageCubeArray << "\n";
    os << "independentBlend: " << features.independentBlend << "\n";
    os << "geometryShader: " << features.geometryShader << "\n";
    os << "tessellationShader: " << features.tessellationShader << "\n";
    os << "sampleRateShading: " << features.sampleRateShading << "\n";
    os << "dualSrcBlend: " << features.dualSrcBlend << "\n";
    os << "logicOp: " << features.logicOp << "\n";
    os << "multiDrawIndirect: " << features.multiDrawIndirect << "\n";
    os << "drawIndirectFirstInstance: " << features.drawIndirectFirstInstance
       << "\n";
    os << "depthClamp: " << features.depthClamp << "\n";
    os << "depthBiasClamp: " << features.depthBiasClamp << "\n";
    os << "fillModeNonSolid: " << features.fillModeNonSolid << "\n";
    os << "depthBounds: " << features.depthBounds << "\n";
    os << "wideLines: " << features.wideLines << "\n";
    os << "largePoints: " << features.largePoints << "\n";
    os << "alphaToOne: " << features.alphaToOne << "\n";
    os << "multiViewport: " << features.multiViewport << "\n";
    os << "samplerAnisotropy: " << features.samplerAnisotropy << "\n";
    os << "textureCompressionETC2: " << features.textureCompressionETC2 << "\n";
    os << "textureCompressionASTC_LDR: " << features.textureCompressionASTC_LDR
       << "\n";
    os << "textureCompressionBC: " << features.textureCompressionBC << "\n";
    os << "occlusionQueryPrecise: " << features.occlusionQueryPrecise << "\n";
    os << "pipelineStatisticsQuery: " << features.pipelineStatisticsQuery
       << "\n";
    os << "vertexPipelineStoresAndAtomics: "
       << features.vertexPipelineStoresAndAtomics << "\n";
    os << "fragmentStoresAndAtomics: " << features.fragmentStoresAndAtomics
       << "\n";
    os << "shaderTessellationAndGeometryPointSize: "
       << features.shaderTessellationAndGeometryPointSize << "\n";
    os << "shaderImageGatherExtended: " << features.shaderImageGatherExtended
       << "\n";
    os << "shaderStorageImageExtendedFormats: "
       << features.shaderStorageImageExtendedFormats << "\n";
    os << "shaderStorageImageMultisample: "
       << features.shaderStorageImageMultisample << "\n";
    os << "shaderStorageImageReadWithoutFormat: "
       << features.shaderStorageImageReadWithoutFormat << "\n";
    os << "shaderStorageImageWriteWithoutFormat: "
       << features.shaderStorageImageWriteWithoutFormat << "\n";
    os << "shaderUniformBufferArrayDynamicIndexing: "
       << features.shaderUniformBufferArrayDynamicIndexing << "\n";
    os << "shaderSampledImageArrayDynamicIndexing: "
       << features.shaderSampledImageArrayDynamicIndexing << "\n";
    os << "shaderStorageBufferArrayDynamicIndexing: "
       << features.shaderStorageBufferArrayDynamicIndexing << "\n";
    os << "shaderStorageImageArrayDynamicIndexing: "
       << features.shaderStorageImageArrayDynamicIndexing << "\n";
    os << "shaderClipDistance: " << features.shaderClipDistance << "\n";
    os << "shaderCullDistance: " << features.shaderCullDistance << "\n";
    os << "shaderFloat64: " << features.shaderFloat64 << "\n";
    os << "shaderInt64: " << features.shaderInt64 << "\n";
    os << "shaderInt16: " << features.shaderInt16 << "\n";
    os << "shaderResourceResidency: " << features.shaderResourceResidency
       << "\n";
    os << "shaderResourceMinLod: " << features.shaderResourceMinLod << "\n";
    os << "sparseBinding: " << features.sparseBinding << "\n";
    os << "sparseResidencyBuffer: " << features.sparseResidencyBuffer << "\n";
    os << "sparseResidencyImage2D: " << features.sparseResidencyImage2D << "\n";
    os << "sparseResidencyImage3D: " << features.sparseResidencyImage3D << "\n";
    os << "sparseResidency2Samples: " << features.sparseResidency2Samples
       << "\n";
    os << "sparseResidency4Samples: " << features.sparseResidency4Samples
       << "\n";
    os << "sparseResidency8Samples: " << features.sparseResidency8Samples
       << "\n";
    os << "sparseResidency16Samples: " << features.sparseResidency16Samples
       << "\n";
    os << "sparseResidencyAliased: " << features.sparseResidencyAliased << "\n";
    os << "variableMultisampleRate: " << features.variableMultisampleRate
       << "\n";
    os << "inheritedQueries: " << features.inheritedQueries << "\n";

    return os;
}

void render::get_physical_device()
{
    using namespace std::ranges;
    std::vector<vk::raii::PhysicalDevice> physical_devices =
        instance.enumeratePhysicalDevices();

    if (physical_devices.empty())
    {
        throw std::runtime_error(
            "error: no any vulkan physical device found in the system");
    }

    log << "vulkan physical devises in the system:\n";
    for_each(physical_devices,
             [this](const vk::raii::PhysicalDevice& device)
             {
                 const auto& properties = device.getProperties();
                 log << properties << '\n';
                 const auto& features = device.getFeatures();
                 log << features << '\n';
             });
    // find first suitable device
    auto it = find_if(physical_devices,
                      [this](const vk::PhysicalDevice& physical)
                      { return check_device_suitable(physical); });
    if (it == physical_devices.end())
    {
        throw std::runtime_error(
            "error: no physical devices found with render queue");
    }

    devices.physical = std::move(*it);

    log << "selected device: " << devices.physical.getProperties().deviceName
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
    vk::SurfaceKHR surfaceKHR =
        platform.create_vulkan_surface(instance, nullptr);
    if (!surfaceKHR)
    {
        throw std::runtime_error(
            "error: can't create VkSurfaceKHR from user provided callback");
    }

    log << "vk surface KHR created\n";
    surface = vk::raii::SurfaceKHR(instance, surfaceKHR);
}

std::ostream& operator<<(std::ostream&                      os,
                         const render::swapchain_details_t& details)
{
    os << "swap_chain_details:\n";
    auto& caps = details.surface_capabilities;
    os << "surface_capabilities:\n"
       << "    Min image count: " << caps.minImageCount << "\n"
       << "    Max image count: " << caps.maxImageCount << "\n"
       << "    Current extent: " << caps.currentExtent.width << "x"
       << caps.currentExtent.height << "\n"
       << "    Min image extent: " << caps.minImageExtent.width << "x"
       << caps.minImageExtent.height << "\n"
       << "    Max image extent: " << caps.maxImageExtent.width << "x"
       << caps.maxImageExtent.height << "\n"
       << "    Max image array layers: " << caps.maxImageArrayLayers << "\n"
       << "    Supported transformation: "
       << vk::to_string(caps.currentTransform) << "\n"
       << "    Composite alpha flags: "
       << vk::to_string(caps.supportedCompositeAlpha) << "\n"
       << "    Supported usage flags: "
       << vk::to_string(caps.supportedUsageFlags) << "\n";

    os << "surface formats:\n";
    std::ranges::for_each(details.surface_formats,
                          [&os](vk::SurfaceFormatKHR format)
                          {
                              os << "    Image format: "
                                 << vk::to_string(format.format) << "\n"
                                 << "    Color space: "
                                 << vk::to_string(format.colorSpace) << "\n";
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
    swapchain_details_t swap_chain_details =
        get_swapchain_details(*devices.physical);

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
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
        devices.physical.getQueueFamilyProperties();
    // get the first index into queueFamilyProperties which supports both
    // graphics and present
    queue_family.index.graphics = ~0;
    for (uint32_t qfp_index = 0; qfp_index < queueFamilyProperties.size();
         qfp_index++)
    {
        vk::QueueFamilyProperties& properties =
            queueFamilyProperties.at(qfp_index);
        if ((properties.queueFlags & vk::QueueFlagBits::eGraphics) &&
            devices.physical.getSurfaceSupportKHR(qfp_index, *surface))
        {
            // found a queue family that supports both graphics and present
            queue_family.index.graphics     = qfp_index;
            queue_family.index.presentation = qfp_index;
            break;
        }
    }
    if (queue_family.index.graphics == ~0)
    {
        throw std::runtime_error("error: could not find a queue for graphics "
                                 "and present -> terminating");
    }

    float priorities = 0.f; // should be in [0..1] 1 - hi, 0 - lowest
    vk::DeviceQueueCreateInfo device_queue_create_info = {
        .queueFamilyIndex = queue_family.index.graphics,
        .queueCount       = 1u,
        .pQueuePriorities = &priorities
    };

    // Create a chain of feature structures
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan13Features,
                       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        feature_chain = {
            {}, // vk::PhysicalDeviceFeatures2 (empty for now)
            { .dynamicRendering =
                  true }, // Enable dynamic rendering from Vulkan 1.3
            { .extendedDynamicState =
                  true } // Enable extended dynamic state from the extension
        };

    vk::DeviceCreateInfo device_create_info{
        .pNext = &feature_chain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos    = &device_queue_create_info,
        .enabledLayerCount    = 0, // in vk_1_1+ this takes from vk::instance
        .enabledExtensionCount =
            static_cast<uint32_t>(required_device_extensions.size()),
        .ppEnabledExtensionNames = required_device_extensions.data(),
    };

    devices.logical = vk::raii::Device(devices.physical, device_create_info);
    log << "logical device created\n";

    uint32_t queue_index = 0; // Because we’re only creating a single queue from
                              // this family, we’ll simply use index 0
    graphics_queue = vk::raii::Queue(
        devices.logical, queue_family.index.graphics, queue_index);
    log << "got render queue\n";

    if (queue_family.index.graphics != queue_family.index.presentation)
    {
        presentation_queue = vk::raii::Queue(
            devices.logical, queue_family.index.presentation, queue_index);
        log << "got presentation queue\n";
    }
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
    log << swapchain_details.surface_capabilities << '\n';

    uint32_t image_count = std::max(3u, surface_capabilities.minImageCount);
    // if 0 - then limitless
    if (surface_capabilities.maxImageCount > 0u &&
        image_count > surface_capabilities.maxImageCount)
    {
        image_count = surface_capabilities.maxImageCount;
    }

    log << "image_count in swapchain minImageCount (triple buffering "
           "expected): "
        << image_count << std::endl;

    // The .imageArrayLayers specifies the number of layers each image consists
    // of. This is always 1 unless you are developing a stereoscopic 3D
    // application.
    vk::SwapchainCreateInfoKHR create_info{
        .flags            = vk::SwapchainCreateFlagsKHR(),
        .surface          = surface,
        .minImageCount    = image_count,
        .imageFormat      = selected_format.format,
        .imageColorSpace  = selected_format.colorSpace,
        .imageExtent      = selected_image_resolution,
        .imageArrayLayers = 1u, // number of layers for each image in chain
        .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform     = surface_capabilities.currentTransform,
        .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode      = selected_presentation_mode,
        .clipped          = true,
        .oldSwapchain     = nullptr
    };

    // if Graphics and Presentation families are different, then swapchain
    // must let images be shared between families
    if (queue_family.index.graphics != queue_family.index.presentation)
    {
        log << "graphics_family != presentation_family use sharing_mode: "
            << vk::to_string(vk::SharingMode::eConcurrent) << std::endl;
        create_info.imageSharingMode      = vk::SharingMode::eConcurrent;
        create_info.pQueueFamilyIndices   = queue_family.array.data();
        create_info.queueFamilyIndexCount = queue_family.array.size();
    }
    else
    {
        create_info.imageSharingMode      = vk::SharingMode::eExclusive;
        create_info.pQueueFamilyIndices   = nullptr; // optional
        create_info.queueFamilyIndexCount = 0;       // optional
    }

    // if old swapchain been destroyed and this one replaces it, then link
    // old one to quickly hand over responsibilities
    create_info.oldSwapchain = nullptr;
    create_info.surface      = surface;

    swapchain = devices.logical.createSwapchainKHR(create_info);
    log << "vulkan swapchain created\n";
    set_object_name(*swapchain, "om_swapchain");

    // store for later usages
    swapchain_image_format = create_info.imageFormat;
    swapchain_image_extent = create_info.imageExtent;
    log << "store in utilities swapchain_image_format: "
        << vk::to_string(swapchain_image_format) << '\n'
        << "swapchain_image_extent: " << swapchain_image_extent.width << 'x'
        << swapchain_image_extent.height << std::endl;

    swapchain_images = (*devices.logical).getSwapchainImagesKHR(swapchain);
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

    set_object_name(*render_path, "only_render_path");
}

void render::create_graphics_pipeline()
{
    // Static Pipeline States
    auto vertex_shader_code = platform.get_file_content(
        "./02-vulkan/09-vk-res-loading-1/shaders/shader.vert.slang.spv");
    auto fragment_shader_code = platform.get_file_content(
        "./02-vulkan/09-vk-res-loading-1/shaders/shader.frag.slang.spv");
    // compile shaders from spir-v into gpu code
    vk::ShaderModule vertex = create_shader(vertex_shader_code.as_span());
    std::experimental::scope_exit vertex_cleanup([this, &vertex]()
                                                 { destroy(vertex); });

    vk::ShaderModule fragment = create_shader(fragment_shader_code.as_span());
    std::experimental::scope_exit fragment_cleanup([this, &fragment]()
                                                   { destroy(fragment); });

    vk::PipelineShaderStageCreateInfo stage_info_vert{
        .stage  = vk::ShaderStageFlagBits::eVertex,
        .module = vertex,
        .pName  = "main"
    };
    vk::PipelineShaderStageCreateInfo stage_info_frag{
        .stage  = vk::ShaderStageFlagBits::eFragment,
        .module = fragment,
        .pName  = "main"
    };

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
    scissor.offset = { .x = 0, .y = 0 };     // offset to use region from
    scissor.extent = swapchain_image_extent; // region extent

    vk::PipelineViewportStateCreateInfo viewport_state_info{ .viewportCount = 1,
                                                             .pViewports =
                                                                 &viewport,
                                                             .scissorCount = 1,
                                                             .pScissors =
                                                                 &scissor };
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

    graphics_pipeline =
        devices.logical.createGraphicsPipeline(nullptr, graphics_info);

    set_object_name(*graphics_pipeline, "only_graphics_pipeline");
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
    log << "create command pool\n";

    vk::CommandPoolCreateInfo info{};
    info.queueFamilyIndex = 0; /* queue_indexes.graphics_family; */
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

    command_buffers = (*devices.logical).allocateCommandBuffers(info);

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
    synchronization.image_available.resize(swapchain_images.size());
    synchronization.render_finished.resize(swapchain_images.size());
    synchronization.gpu_fence.resize(swapchain_images.size());

    struct gen_sem
    {
        render&     self;
        std::string name;
        int         index;

        vk::Semaphore operator()()
        {
            auto semaphore = (*self.devices.logical).createSemaphore({});
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
                              auto fence = (*devices.logical).createFence(info);
                              set_object_name(fence,
                                              "fence_" + std::to_string(i++));
                              return fence;
                          });
}

void render::destroy_synchronization_objects() noexcept
{
    auto destroy_sem = [this](vk::Semaphore& semaphore)
    { (*devices.logical).destroy(semaphore); };

    std::ranges::for_each(synchronization.image_available, destroy_sem);
    std::ranges::for_each(synchronization.render_finished, destroy_sem);

    std::ranges::for_each(synchronization.gpu_fence,
                          [this](vk::Fence& fence)
                          { (*devices.logical).destroy(fence); });
}

vk::Extent2D render::choose_best_swapchain_image_resolution(
    const vk::SurfaceCapabilitiesKHR& capabilities)
{
    auto extent = capabilities.currentExtent;
    if (extent.width != std::numeric_limits<uint32_t>::max())
    {
        log << "use Extent2D from surface\n";
        // no need to clamp
        return extent;
    }

    platform_interface::buffer_size buffer_size =
        platform.get_window_buffer_size();

    extent.width  = buffer_size.width;
    extent.height = buffer_size.height;

    log << "use Extent2D from callback_get_window_buffer_size: "
        << buffer_size.width << 'x' << buffer_size.height << std::endl;

    return { std::clamp(extent.width,
                        capabilities.minImageExtent.width,
                        capabilities.maxImageExtent.width),
             std::clamp(extent.height,
                        capabilities.minImageExtent.height,
                        capabilities.maxImageExtent.height) };
}

vk::SurfaceFormatKHR render::choose_best_surface_format(
    std::span<vk::SurfaceFormatKHR> formats)
{
    if (formats.empty())
    {
        throw std::runtime_error("empty surface formats");
    }

    vk::SurfaceFormatKHR rgba32_srgb(vk::Format::eR8G8B8A8Srgb,
                                     vk::ColorSpaceKHR::eSrgbNonlinear);
    vk::SurfaceFormatKHR bgra32_srgb(vk::Format::eB8G8R8A8Srgb,
                                     vk::ColorSpaceKHR::eSrgbNonlinear);

    if (formats.size() == 1 && formats.front().format == vk::Format::eUndefined)
    {
        // this means all formats are supported!
        // so let's use our defaults
        return rgba32_srgb;
    }
    // not all supported search for RGB or BGR
    std::array<vk::SurfaceFormatKHR, 2> suitable_formats = { bgra32_srgb,
                                                             rgba32_srgb };
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
        // This is another variation of the vk::PresentModeKHR::eFifo mode.
        // Instead of blocking the application when the queue is full, the
        // images that are already queued are simply replaced with the newer
        // ones. This mode can be used to render frames as fast as possible
        // while still avoiding tearing, resulting in fewer latency issues than
        // standard vertical sync. This is commonly known as "triple buffering,"
        // although the existence of three buffers alone does not necessarily
        // mean that the framerate is unlocked.
        // On mobile devices, where energy usage is more important, you will
        // probably want to use vk::PresentModeKHR::eFifo instead
        return vk::PresentModeKHR::eMailbox;
    }
    // The swap chain is a queue where the display takes an image from the front
    // of the queue when the display is refreshed, and the program inserts
    // rendered images at the back of the queue. If the queue is full, then the
    // program has to wait. This is most similar to vertical sync as found in
    // modern games. The moment that the display is refreshed is known as
    // "vertical blank". Guaranteed to be in any vulkan implementation
    return vk::PresentModeKHR::eFifo;
}

render::swapchain_details_t render::get_swapchain_details(
    const vk::PhysicalDevice& device)
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
    vk::ShaderModuleCreateInfo create_info{
        .codeSize = spir_v.size(),
        .pCode    = reinterpret_cast<const uint32_t*>(spir_v.data())
    };

    return devices.logical.createShaderModule(create_info);
}

void render::destroy_surface() noexcept
{
    platform.destroy_vulkan_surface(instance, surface, nullptr);
    log << "vulkan surface destroyed\n";
}

void render::destroy(vk::ShaderModule& shader) noexcept
{
    log << "destroy shader module\n";
    (*devices.logical).destroy(shader);
}
} // namespace om::vulkan
