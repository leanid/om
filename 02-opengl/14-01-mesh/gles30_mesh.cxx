#include "gles30_mesh.hxx"

#include <cstddef>

#include "opengles30.hxx"

namespace gles30
{

mesh::mesh(mesh&& other) noexcept
    : vertices{ std::move(other.vertices) }
    , indices{ std::move(other.indices) }
    , textures{ std::move(other.textures) }
    , VAO{ 0 }
    , VBO{ 0 }
    , EBO{ 0 }
{
    std::swap(VBO, other.VBO);
    std::swap(EBO, other.EBO);
    std::swap(VAO, other.VAO);
}

void swap(mesh& l, mesh& r) noexcept
{
    using namespace std;
    swap(l.vertices, r.vertices);
    swap(l.indices, r.indices);
    swap(l.textures, r.textures);
    std::swap(l.EBO, r.EBO);
    std::swap(l.VBO, r.VBO);
    std::swap(l.VAO, r.VAO);
}

mesh& mesh::operator=(mesh&& other) noexcept
{
    mesh tmp(std::move(other));
    swap(tmp, *this);
    return *this;
}

mesh::~mesh() noexcept
{
    glDeleteBuffers(1, &VAO);
    gl_check();

    glDeleteBuffers(1, &VBO);
    gl_check();
}

void mesh::draw(shader& shader)
{
    shader.use();
    uint32_t diffuseNr  = 1;
    uint32_t specularNr = 1;
    for (uint32_t i = 0; i < textures.size(); i++)
    {
        // activate proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + i);
        gl_check();
        // retrieve texture number (the N in diffuse_textureN)
        texture&         texture = *textures.at(i);
        texture::uv_type type    = texture.get_type();
        char             str[32];

        int32_t is_ok = 0;

        if (type == texture::uv_type::diffuse)
        {
            is_ok = snprintf(str, sizeof(str), "tex_diffuse%d", diffuseNr++);
            assert(is_ok > 0);
        }
        else if (type == texture::uv_type::specular)
        {
            is_ok = snprintf(str, sizeof(str), "tex_specular%d", specularNr++);
            assert(is_ok > 0);
        }

        char mat_name[64];
        is_ok = snprintf(mat_name, sizeof(mat_name), "material.%32s", str);
        assert(is_ok > 0);

        shader.set_uniform(mat_name, static_cast<int32_t>(i));
        texture.bind();
    }

    glActiveTexture(GL_TEXTURE0);
    gl_check();

    // draw mesh
    glBindVertexArray(VAO);
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
    assert(VBO == 0);
    assert(EBO == 0);
    assert(VAO == 0);

    glGenVertexArrays(1, &VAO);
    gl_check();
    glGenBuffers(1, &VBO);
    gl_check();
    glGenBuffers(1, &EBO);
    gl_check();

    glBindVertexArray(VAO);
    gl_check();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl_check();

    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<signed>(vertices.size() * sizeof(vertex)),
                 vertices.data(),
                 GL_STATIC_DRAW);
    gl_check();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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
