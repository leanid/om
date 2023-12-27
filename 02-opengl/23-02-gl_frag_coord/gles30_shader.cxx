#include "gles30_shader.hxx"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "gles30_texture.hxx"
#include "opengles30.hxx"

namespace gles30
{
void shader::create(std::string_view vertex_shader_src,
                    std::string_view fragment_shader_src)
{
    // create OpenGL object id for vertex shader object
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    // load vertex shader source code into vertex_shader
    const GLchar* array_of_pointers_to_strings_with_src[1];
    array_of_pointers_to_strings_with_src[0] = vertex_shader_src.data();

    GLint array_of_string_lengths[1];
    array_of_string_lengths[0] = static_cast<GLint>(vertex_shader_src.size());

    glShaderSource(vertex_shader,
                   1,
                   array_of_pointers_to_strings_with_src,
                   array_of_string_lengths);

    // compile vertex shader
    glCompileShader(vertex_shader);

    // check compilation status of our shader
    int  success;
    char info_log[1024] = { 0 };
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (0 == success)
    {
        glGetShaderInfoLog(vertex_shader, sizeof(info_log), nullptr, info_log);

        std::stringstream ss;
        ss << "error: in vertex shader: " << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }

    // generate new id for shader object
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // load fragment shader source code
    array_of_pointers_to_strings_with_src[0] = fragment_shader_src.data();
    array_of_string_lengths[0] = static_cast<GLint>(fragment_shader_src.size());
    glShaderSource(fragment_shader,
                   1,
                   array_of_pointers_to_strings_with_src,
                   array_of_string_lengths);

    glCompileShader(fragment_shader);

    // check compilation status of our shader
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (0 == success)
    {
        glGetShaderInfoLog(
            fragment_shader, sizeof(info_log), nullptr, info_log);

        std::stringstream ss;
        ss << "error: in fragment shader: " << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }

    // create complete shader program and reseive id (vertex + geometry +
    // fragment) geometry shader - will have default value
    program_id = glCreateProgram();

    glAttachShader(program_id, vertex_shader);

    glAttachShader(program_id, fragment_shader);

    // no link program like in c/c++ object files
    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &success);

    if (0 == success)
    {
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log);

        std::stringstream ss;
        ss << "error: linking: " << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }

    // after linking shader program we don't need object parts of it
    // so we can free OpenGL memory and delete vertex and fragment parts
    glDeleteShader(vertex_shader);

    glDeleteShader(fragment_shader);
}

shader::shader(
    const std::filesystem::path& vertex_shader_path,
    const std::filesystem::path& fragment_shader_path) noexcept(false)
    : program_id(0)
{
    std::ifstream vertex_stream;
    std::ifstream fragmen_stream;
    vertex_stream.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    fragmen_stream.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    try
    {
        try
        {
            vertex_stream.open(vertex_shader_path);
        }
        catch (...)
        {
            std::cerr << "failed to open: " << vertex_shader_path << std::endl;
            throw;
        }

        try
        {
            fragmen_stream.open(fragment_shader_path);
        }
        catch (...)
        {
            std::cerr << "faild to open: " << fragment_shader_path << std::endl;
            throw;
        }

        std::stringstream vertex_src;
        vertex_src << vertex_stream.rdbuf();
        std::stringstream fragment_src;
        fragment_src << fragmen_stream.rdbuf();

        std::string v_src{ vertex_src.str() };
        std::string f_src{ fragment_src.str() };

        std::string_view v{ v_src };
        std::string_view f{ f_src };

        create(v, f);
    }
    catch (const std::exception& e)
    {
        std::clog << e.what() << std::endl;

        throw std::runtime_error(
            "can't create shader from: " + vertex_shader_path.u8string() + " " +
            fragment_shader_path.u8string() + " cause: " + e.what());
    }
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
shader::~shader() noexcept
{
    glDeleteProgram(program_id); // 0 will be silently ignored
}

void shader::use()
{
    assert(GL_TRUE == glIsProgram(program_id));
    glUseProgram(program_id);
}

GLint get_uniform_index(std::string_view name, uint32_t program_id)
{
    assert(*(name.data() + name.size()) == '\0'); // null terminated string
    GLint uniform_index = glGetUniformLocation(program_id, name.data());

    if (uniform_index == -1)
    {
        throw std::runtime_error("can't find uniform: " + std::string(name));
    }
    return uniform_index;
}

void shader::set_uniform(std::string_view name, bool value)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniform1i(uniform_index, value);
}
void shader::set_uniform(std::string_view name, std::int32_t value)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniform1i(uniform_index, value);
}
void shader::set_uniform(std::string_view name, float value)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniform1f(uniform_index, value);
}
void shader::set_uniform(std::string_view name,
                         texture&         tex,
                         std::uint32_t    index)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glActiveTexture(GL_TEXTURE0 + index);

    tex.bind();
    glUniform1i(uniform_index, static_cast<int32_t>(index));
}
void shader::set_uniform(std::string_view name, const glm::mat4& m)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniformMatrix4fv(uniform_index, 1, GL_FALSE, glm::value_ptr(m));
}
void shader::set_uniform(std::string_view name, const glm::mat3& m)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniformMatrix3fv(uniform_index, 1, GL_FALSE, glm::value_ptr(m));
}

void shader::set_uniform(std::string_view name, const glm::vec3& v)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniform3fv(uniform_index, 1, glm::value_ptr(v));
}

void shader::set_uniform(std::string_view name, const glm::vec2& v)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniform2fv(uniform_index, 1, glm::value_ptr(v));
}

std::string shader::validate() noexcept(false)
{
    // validate current OpenGL state
    glValidateProgram(program_id);

    int success;
    glGetProgramiv(program_id, GL_VALIDATE_STATUS, &success);

    if (1 == success)
    {
        char info_log[4096] = { 0 };
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log);

        if (strlen(info_log) > 0)
        {
            return { info_log };
        }
    }
    else
    {
        throw std::runtime_error("can't get validation status");
    }
    return {};
}
} // namespace gles30
