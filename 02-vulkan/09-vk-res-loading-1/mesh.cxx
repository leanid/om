#include "mesh.hxx"

#include <iostream>

namespace om::vulkan
{
mesh::mesh(vk::PhysicalDevice physical_device,
           vk::Device         device,
           std::span<vertex>  vertexes)
    : physical_device(physical_device)
    , device(device)
    , buffer()
    , vertex_buf_mem()
    , num_vertexes(vertexes.size())
{
    create_buffer(vertexes);
}

uint32_t mesh::get_vertex_count() const
{
    return num_vertexes;
}

vk::Buffer mesh::get_vertex_buffer()
{
    return buffer;
}
void mesh::create_buffer(std::span<vertex> vertexes)
{
    // information to create buffer (doesn't include assining memory)
    vk::BufferCreateInfo info;
    info.size        = sizeof(vertex) * vertexes.size();
    info.usage       = vk::BufferUsageFlagBits::eVertexBuffer;
    info.sharingMode = vk::SharingMode::eExclusive; // similar to swapchain
                                                    // images, can share vertex
                                                    // buffers

    buffer = device.createBuffer(info);

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
    if (!num_vertexes)
    {
        return;
    }
    std::cout << "mesh::cleanup" << std::endl;
    if (buffer)
    {
        std::cout << "destroy mesh buffer" << std::endl;
        device.destroyBuffer(buffer);
    }
    if (vertex_buf_mem)
    {
        std::cout << "destroy mesh vertex_buf_mem" << std::endl;
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
