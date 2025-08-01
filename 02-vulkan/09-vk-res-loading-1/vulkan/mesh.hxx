#pragma once

#include <span>

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace om::vulkan
{
struct vertex final
{
    glm::vec3 pos; // vertex positions x, y, z
    glm::vec3 col; // vertex color r, g, b
};

class mesh final
{
public:
    mesh() = default;
    mesh(vk::PhysicalDevice physical_device,
         vk::Device         device,
         std::span<vertex>  vertexes);
    mesh(const mesh& other)            = delete;
    mesh& operator=(const mesh& other) = delete;
    mesh(mesh&& other);
    mesh& operator=(mesh&& other);
    ~mesh();

    [[nodiscard]] uint32_t get_vertex_count() const;
    vk::Buffer             get_vertex_buffer();

    void cleanup() noexcept;

private:
    void     create_buffer(std::span<vertex> vertexes);
    uint32_t find_mem_type_index(uint32_t                allowed_types,
                                 vk::MemoryPropertyFlags properties);

    vk::PhysicalDevice physical_device;
    vk::Device         device;
    vk::Buffer         buffer;
    vk::DeviceMemory   vertex_buf_mem;
    uint32_t           num_vertexes{};
};
} // namespace om::vulkan
