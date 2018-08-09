#include "gles30_shader.hxx"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <sstream>
#include <utility>

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

    glShaderSource(vertex_shader, 1, array_of_pointers_to_strings_with_src,
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
    array_of_string_lengths[0]               = fragment_shader_src.size();
    glShaderSource(fragment_shader, 1, array_of_pointers_to_strings_with_src,
                   array_of_string_lengths);
    gl_check();

    glCompileShader(fragment_shader);
    gl_check();

    // check compilation status of our shader
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    gl_check();

    if (0 == success)
    {
        glGetShaderInfoLog(fragment_shader, sizeof(info_log), nullptr,
                           info_log);
        gl_check();

        std::stringstream ss;
        ss << "error: in fragment shader: " << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }

    // create complete shader program and reseive id (vertex + geometry +
    // fragment) geometry shader - will have default value
    uint32_t shader_program;
    shader_program = glCreateProgram();
    gl_check();

    glAttachShader(shader_program, vertex_shader);
    gl_check();

    glAttachShader(shader_program, fragment_shader);
    gl_check();

    // no link program like in c/c++ object files
    glLinkProgram(shader_program);
    gl_check();

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    gl_check();

    if (0 == success)
    {
        glGetProgramInfoLog(shader_program, sizeof(info_log), nullptr,
                            info_log);
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
    glDeleteProgram(program_id); // 0 will be silently ignored
    gl_check();
}

void shader::use()
{
    glUseProgram(program_id);
    gl_check();
}

GLint get_uniform_index(std::string_view name, uint32_t program_id)
{
    assert(*(name.data() + name.size()) == '\0'); // null terminated string
    GLint uniform_index = glGetUniformLocation(program_id, name.data());
    gl_check();
    assert(uniform_index != -1);
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

void shader::validate() noexcept(false)
{
    // validate current OpenGL state
    glValidateProgram(program_id);
    gl_check();

    int success;
    glGetProgramiv(program_id, GL_VALIDATE_STATUS, &success);
    gl_check();

    if (0 == success)
    {
        char info_log[4096] = { 0 };
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log);
        gl_check();

        if (strlen(info_log) > 0)
        {
            throw std::runtime_error(info_log);
        }
    }
    else
    {
        throw std::runtime_error("can't get validation status");
    }
}
} // namespace gles30
