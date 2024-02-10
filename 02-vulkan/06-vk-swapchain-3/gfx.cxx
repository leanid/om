#include "gfx.hxx"

namespace om
{

gfx::gfx(std::ostream&                   log,
         callback_get_ext                get_instance_extensions,
         const callback_create_surface&  create_vk_surface,
         callback_get_window_buffer_size get_window_buffer_size,
         hints                           h)
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

gfx::~gfx()
{
    std::ranges::for_each(swapchain_image_views,
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

void gfx::create_instance(callback_get_ext get_instance_extensions)
{
    vk::ApplicationInfo    application_info;
    vk::InstanceCreateInfo instance_create_info;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.ppEnabledExtensionNames =
        get_instance_extensions(&instance_create_info.enabledExtensionCount);

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

void gfx::validate_expected_extensions_exists(
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

} // namespace om
