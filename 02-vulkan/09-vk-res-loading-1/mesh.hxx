#pragma once

#include "render.hxx"

namespace om::vulkan
{
class mesh final
{
public:
    mesh() = default;
    mesh(vk::PhysicalDevice   physical_device,
         vk::Device           device,
         std::vector<vertex>& vertexes);
    ~mesh();

    [[nodiscard]] uint32_t get_vertex_count() const;
    vk::Buffer             get_vertex_buffer();

    void cleanup() const noexcept;

private:
    vk::Buffer create_buffer(std::vector<vertex>& vertexes);
    uint32_t   find_mem_type_index(uint32_t                allowed_types,
                                   vk::MemoryPropertyFlags properties);

    vk::PhysicalDevice physical_device;
    vk::Device         device;
    vk::Buffer         buffer;
    uint32_t           num_vertexes{};
};
} // namespace om::vulkan
