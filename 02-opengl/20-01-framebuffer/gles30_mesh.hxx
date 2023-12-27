#pragma once

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

private:
    friend void swap(mesh& l, mesh& r) noexcept;
    void        setup();

    std::vector<vertex>   vertices;
    std::vector<uint32_t> indices;
    std::vector<texture*> textures;

    uint32_t vao{ 0 }; // vertex array object (store format and vbo's info)
    uint32_t vbo{ 0 }; // vertex buffer object
    uint32_t ebo{ 0 }; // element buffer object (index)
};

inline mesh::mesh(std::vector<vertex>   a_vertices,
                  std::vector<uint32_t> a_indices,
                  std::vector<texture*> a_textures)
    : vertices{ std::move(a_vertices) }
    , indices{ std::move(a_indices) }
    , textures{ std::move(a_textures) }
{
    setup();
}

} // namespace gles30
