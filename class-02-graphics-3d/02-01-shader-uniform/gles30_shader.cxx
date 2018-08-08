#include "gles30_shader.hxx"

#include <algorithm>
#include <cstdint>
#include <utility>

#include "opengles30.hxx"

namespace gles30
{
shader::shader(std::string_view vertex_shader_src,
               std::string_view fragment_shader_src)
    : program_id(0)
{
    // TODO
}
shader::shader(shader&& other) noexcept
    : program_id(other.program_id)
{
    other.program_id = 0;
}

shader& shader::operator=(shader&& other) noexcept
{
    shader tmp(std::move(other));

    std::swap(tmp.program_id, program_id);
    return *this;
}
shader::~shader()
{
    if (program_id != 0)
    {
        glDeleteProgram(program_id);
        gl_check();
    }
}

void shader::use() {}

void shader::set_uniform(std::string_view name, bool value) {}
void shader::set_uniform(std::string_view name, std::int32_t value) {}
void shader::set_uniform(std::string_view name, float value) {}
} // namespace gles30
