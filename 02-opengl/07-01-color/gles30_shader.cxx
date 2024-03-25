#include "gles30_shader.hxx"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <utility>

#include "gles30_texture.hxx"
#include "opengles30.hxx"

namespace gles30
{
shader::shader(std::string_view vertex_shader_src,
               std::string_view fragment_shader_src)
    : program_id(0)
{
    // create OpenGL object id for vertex shader object
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    gl_check();

    // load vertex shader source code into vertex_shader
    const GLchar* array_of_pointers_to_strings_with_src[1];
    array_of_pointers_to_strings_with_src[0] = vertex_shader_src.data();

    GLint array_of_string_lengths[1];
    array_of_string_lengths[0] = static_cast<GLint>(vertex_shader_src.size());

    glShaderSource(vertex_shader,
                   1,
                   array_of_pointers_to_strings_with_src,
                   array_of_string_lengths);
    gl_check();

    // compile vertex shader
    glCompileShader(vertex_shader);
    gl_check();

    // check compilation status of our shader
    int  success;
    char info_log[1024] = { 0 };
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    gl_check();

    if (0 == success)
    {
        glGetShaderInfoLog(vertex_shader, sizeof(info_log), nullptr, info_log);
        gl_check();

        std::stringstream ss;
        ss << "error: in vertex shader: " << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }

    // generate new id for shader object
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    gl_check();
    // load fragment shader source code
    array_of_pointers_to_strings_with_src[0] = fragment_shader_src.data();
    array_of_string_lengths[0] = static_cast<GLint>(fragment_shader_src.size());
    glShaderSource(fragment_shader,
                   1,
                   array_of_pointers_to_strings_with_src,
                   array_of_string_lengths);
    gl_check();

    glCompileShader(fragment_shader);
    gl_check();

    // check compilation status of our shader
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    gl_check();

    if (0 == success)
    {
        glGetShaderInfoLog(
            fragment_shader, sizeof(info_log), nullptr, info_log);
        gl_check();

        std::stringstream ss;
        ss << "error: in fragment shader: " << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }

    // create complete shader program and reseive id (vertex + geometry +
    // fragment) geometry shader - will have default value
    program_id = glCreateProgram();
    gl_check();

    glAttachShader(program_id, vertex_shader);
    gl_check();

    glAttachShader(program_id, fragment_shader);
    gl_check();

    // no link program like in c/c++ object files
    glLinkProgram(program_id);
    gl_check();

    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    gl_check();

    if (0 == success)
    {
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log);
        gl_check();

        std::stringstream ss;
        ss << "error: linking: " << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }

    // after linking shader program we don't need object parts of it
    // so we can free OpenGL memory and delete vertex and fragment parts
    glDeleteShader(vertex_shader);
    gl_check();
    glDeleteShader(fragment_shader);
    gl_check();
}

shader::shader(const fs::path& vertex_shader_path,
               const fs::path& fragment_shader_path) noexcept(false)
{
    std::ifstream vertex_stream;
    std::ifstream fragmen_stream;
    vertex_stream.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    fragmen_stream.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    try
    {
        vertex_stream.open(vertex_shader_path);
        fragmen_stream.open(fragment_shader_path);

        std::stringstream vertex_src;
        vertex_src << vertex_stream.rdbuf();
        std::stringstream fragment_src;
        fragment_src << fragmen_stream.rdbuf();

        std::string v_src{ vertex_src.str() };
        std::string f_src{ fragment_src.str() };

        std::string_view v{ v_src };
        std::string_view f{ f_src };

        // placement new operator (call other contructor at existing memory)
        new (this) shader(v, f);
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
    gl_check();
}

void shader::use()
{
    assert(GL_TRUE == glIsProgram(program_id));
    glUseProgram(program_id);
    gl_check();
}

GLint get_uniform_index(std::string_view name, uint32_t program_id)
{
    assert(*(name.data() + name.size()) == '\0'); // null terminated string
    GLint uniform_index = glGetUniformLocation(program_id, name.data());
    gl_check();
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
    gl_check();
}
void shader::set_uniform(std::string_view name, std::int32_t value)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniform1i(uniform_index, value);
    gl_check();
}
void shader::set_uniform(std::string_view name, float value)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniform1f(uniform_index, value);
    gl_check();
}
void shader::set_uniform(std::string_view name,
                         texture&         tex,
                         std::uint32_t    index)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glActiveTexture(GL_TEXTURE0 + index);
    gl_check();
    tex.bind();
    glUniform1i(uniform_index, static_cast<int32_t>(index));
    gl_check();
}
void shader::set_uniform(std::string_view name, const glm::mat4& m)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniformMatrix4fv(uniform_index, 1, GL_FALSE, glm::value_ptr(m));
    gl_check();
}
void shader::set_uniform(std::string_view name, const glm::mat3& m)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniformMatrix3fv(uniform_index, 1, GL_FALSE, glm::value_ptr(m));
    gl_check();
}

void shader::set_uniform(std::string_view name, const glm::vec3& v)
{
    GLint uniform_index = get_uniform_index(name, program_id);
    glUniform3fv(uniform_index, 1, glm::value_ptr(v));
    gl_check();
}

std::string shader::validate() noexcept(false)
{
    // validate current OpenGL state
    glValidateProgram(program_id);
    gl_check();

    int success;
    glGetProgramiv(program_id, GL_VALIDATE_STATUS, &success);
    gl_check();

    if (1 == success)
    {
        char info_log[4096] = { 0 };
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log);
        gl_check();

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
