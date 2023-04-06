#pragma once

#include <functional>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "gles30_shader.hxx"
#include "gles30_texture.hxx"

namespace gles30
{

struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

enum class primitive
{
    points         = 0x0000,
    lines          = 0x0001,
    line_loop      = 0x0002,
    line_strip     = 0x0003,
    triangles      = 0x0004,
    triangle_strip = 0x0005,
    triangle_fan   = 0x0006
};

class mesh
{
public:
    mesh(std::vector<vertex>   a_vertices,
         std::vector<uint32_t> a_indices,
         std::vector<texture*> a_textures);
    mesh(mesh&&) noexcept;
    mesh& operator=(mesh&&) noexcept;
    ~mesh() noexcept;

    mesh(const mesh&)            = delete;
    mesh& operator=(const mesh&) = delete;

    void draw(shader& shader) const;
    void draw_instanced(shader&               shader,
                        size_t                instance_count,
                        std::function<void()> bind_custom_data) const;

    void      set_primitive_type(primitive value);
    primitive get_primitive_type() const;

private:
    friend void swap(mesh& l, mesh& r) noexcept;
    void        setup();

    std::vector<vertex>   vertices;
    std::vector<uint32_t> indices;
    std::vector<texture*> textures;

    uint32_t vao{ 0 }; // vertex array object (store format and vbo's info)
    uint32_t vbo{ 0 }; // vertex buffer object
    uint32_t ebo{ 0 }; // element buffer object (index)

    primitive primitive_type{};
};

inline mesh::mesh(std::vector<vertex>   a_vertices,
                  std::vector<uint32_t> a_indices,
                  std::vector<texture*> a_textures)
    : vertices{ std::move(a_vertices) }
    , indices{ std::move(a_indices) }
    , textures{ std::move(a_textures) }
    , primitive_type{ primitive::triangles }
{
    setup();
}

inline void mesh::set_primitive_type(primitive value)
{
    primitive_type = value;
}

inline primitive mesh::get_primitive_type() const
{
    return primitive_type;
}

} // namespace gles30
