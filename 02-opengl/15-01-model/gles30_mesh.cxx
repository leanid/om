#include "gles30_mesh.hxx"

#include <cstddef>

#include "opengles30.hxx"

namespace gles30
{

mesh::mesh(mesh&& other) noexcept
    : vertices{ std::move(other.vertices) }
    , indices{ std::move(other.indices) }
    , textures{ std::move(other.textures) }
    , vao{ 0 }
    , vbo{ 0 }
    , ebo{ 0 }
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
    gl_check();

    glDeleteBuffers(1, &vbo);
    gl_check();

    glDeleteBuffers(1, &ebo);
    gl_check();
}

void mesh::draw(shader& shader) const
{
    shader.use();
    uint32_t diffuse_index  = 0;
    uint32_t specular_index = 0;
    for (uint32_t i = 0; i < textures.size(); i++)
    {
        // activate proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + i);
        gl_check();
        // retrieve texture number (the N in diffuse_textureN)
        texture& texture = *textures.at(i);
        texture.bind();
        texture::type type = texture.get_type();
        char          str[32];

        int32_t is_ok = 0;

        if (type == texture::type::diffuse)
        {
            is_ok = snprintf(str, sizeof(str), "tex_diffuse%u", diffuse_index);
            assert(is_ok > 0);
            ++diffuse_index;
        }
        else if (type == texture::type::specular)
        {
            is_ok =
                snprintf(str, sizeof(str), "tex_specular%u", specular_index);
            assert(is_ok > 0);
            ++specular_index;
        }

        char tex_uniform_name[64];
        is_ok = snprintf(
            tex_uniform_name, sizeof(tex_uniform_name), "material.%s", str);
        assert(is_ok > 0);

        shader.set_uniform(tex_uniform_name, static_cast<int32_t>(i));
    }

    // draw mesh
    glBindVertexArray(vao);
    gl_check();
    glDrawElements(GL_TRIANGLES,
                   static_cast<signed>(indices.size()),
                   GL_UNSIGNED_INT,
                   nullptr);
    gl_check();
    glBindVertexArray(0);
    gl_check();
}

void mesh::setup()
{
    assert(vbo == 0);
    assert(ebo == 0);
    assert(vao == 0);

    glGenVertexArrays(1, &vao);
    gl_check();
    glGenBuffers(1, &vbo);
    gl_check();
    glGenBuffers(1, &ebo);
    gl_check();

    glBindVertexArray(vao);
    gl_check();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl_check();

    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<signed>(vertices.size() * sizeof(vertex)),
                 vertices.data(),
                 GL_STATIC_DRAW);
    gl_check();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    gl_check();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<signed>(indices.size() * sizeof(uint32_t)),
                 indices.data(),
                 GL_STATIC_DRAW);
    gl_check();

    // vertex positions
    glEnableVertexAttribArray(0);
    gl_check();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), nullptr);
    gl_check();
    // vertex normals
    glEnableVertexAttribArray(1);
    gl_check();
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex),
                          reinterpret_cast<void*>(offsetof(vertex, normal)));
    gl_check();
    // vertex texture coords
    glEnableVertexAttribArray(2);
    gl_check();
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex),
                          reinterpret_cast<void*>(offsetof(vertex, uv)));
    gl_check();

    glBindVertexArray(0);
    gl_check();
}
} // namespace gles30
