#include "gles30_mesh.hxx"

#include <array>
#include <cstddef>

#include "opengles30.hxx"

namespace gles30
{

mesh::mesh(mesh&& other) noexcept
    : vertices{ std::move(other.vertices) }
    , indices{ std::move(other.indices) }
    , textures{ std::move(other.textures) }
{
    std::swap(vbo, other.vbo);
    std::swap(ebo, other.ebo);
    std::swap(vao, other.vao);
}

void swap(mesh& l, mesh& r) noexcept
{
    using namespace std;
    swap(l.vertices, r.vertices);
    swap(l.indices, r.indices);
    swap(l.textures, r.textures);
    std::swap(l.ebo, r.ebo);
    std::swap(l.vbo, r.vbo);
    std::swap(l.vao, r.vao);
}

mesh& mesh::operator=(mesh&& other) noexcept
{
    mesh tmp(std::move(other));
    swap(tmp, *this);
    return *this;
}

mesh::~mesh() noexcept
{
    glDeleteBuffers(1, &vao);

    glDeleteBuffers(1, &vbo);

    glDeleteBuffers(1, &ebo);
}

void mesh::draw(shader& shader) const
{
    shader.use();
    uint32_t diffuse_index  = 0;
    uint32_t specular_index = 0;
    uint32_t cubemap_index  = 0;
    for (uint32_t i = 0; i < textures.size(); i++)
    {
        // activate proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + i);

        // retrieve texture number (the N in diffuse_textureN)
        texture& texture = *textures.at(i);
        texture.bind();
        texture::type type = texture.get_type();
        std::array<char, 32> str{};

        int32_t is_ok = 0;

        if (type == texture::type::diffuse)
        {
            is_ok = snprintf(str.data(), str.size(), "tex_diffuse%u", diffuse_index);
            assert(is_ok > 0);
            ++diffuse_index;
        }
        else if (type == texture::type::specular)
        {
            is_ok =
                snprintf(str.data(), str.size(), "tex_specular%u", specular_index);
            assert(is_ok > 0);
            ++specular_index;
        }
        else if (type == texture::type::cubemap)
        {
            is_ok = snprintf(str.data(), str.size(), "tex_cubemap%u", cubemap_index);
            assert(is_ok > 0);
            ++cubemap_index;
        }

        std::array<char, 64> tex_uniform_name{};
        is_ok = snprintf(
            tex_uniform_name.data(), tex_uniform_name.size(), "material.%s", str.data());
        assert(is_ok > 0);

        shader.set_uniform(tex_uniform_name.data(), static_cast<int32_t>(i));
    }

    // draw mesh
    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES,
                   static_cast<signed>(indices.size()),
                   GL_UNSIGNED_INT,
                   nullptr);

    glBindVertexArray(0);
}

void mesh::setup()
{
    assert(vbo == 0);
    assert(ebo == 0);
    assert(vao == 0);

    glGenVertexArrays(1, &vao);

    glGenBuffers(1, &vbo);

    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<signed>(vertices.size() * sizeof(vertex)),
                 vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<signed>(indices.size() * sizeof(uint32_t)),
                 indices.data(),
                 GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), nullptr);

    // vertex normals
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex),
                          reinterpret_cast<void*>(offsetof(vertex, normal)));

    // vertex texture coords
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex),
                          reinterpret_cast<void*>(offsetof(vertex, uv)));

    glBindVertexArray(0);
}
} // namespace gles30
