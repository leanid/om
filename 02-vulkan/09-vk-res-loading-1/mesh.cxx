#include "mesh.hxx"

namespace om::vulkan
{
mesh::mesh(vk::PhysicalDevice   physical_device,
           vk::Device           device,
           std::vector<vertex>& vertexes)
    : physical_device(physical_device)
    , device(device)
    , buffer(create_buffer(vertexes))
    , num_vertexes(vertexes.size())
{
}

uint32_t mesh::get_vertex_count() const
{
    return num_vertexes;
}

vk::Buffer mesh::get_vertex_buffer()
{
    return buffer;
}
vk::Buffer mesh::create_buffer(std::vector<vertex>& vertexes)
{
    // information to create buffer (doesn't include assining memory)
    vk::BufferCreateInfo info;
    info.size        = sizeof(vertex) * vertexes.size();
    info.usage       = vk::BufferUsageFlagBits::eVertexBuffer;
    info.sharingMode = vk::SharingMode::eExclusive; // similar to swapchain
                                                    // images, can share vertex
                                                    // buffers

    vk::Buffer buffer = device.createBuffer(info);

    // get buffer memory requirements
    vk::MemoryRequirements mem_requirements =
        device.getBufferMemoryRequirements(buffer);

    // allocate memory to buffer
    vk::MemoryAllocateInfo mem_alloc_info;
    mem_alloc_info.allocationSize = mem_requirements.size;

    find_mem_type_index(mem_requirements.memoryTypeBits, )
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
void mesh::cleanup() const noexcept
{
    device.destroyBuffer(buffer);
}
} // namespace om::vulkan
