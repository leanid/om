#include "gles30_shader.hxx"

#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "gles30_texture.hxx"
#include "opengles30.hxx"

namespace gles30
{

struct list_code_with_line_numbers
{
    std::string_view src;
    std::string_view err_msg;
};

std::ostream& operator<<(std::ostream&                      os,
                         const list_code_with_line_numbers& list)
{
    using namespace std;

    const char* open_braket = find(begin(list.err_msg), end(list.err_msg), '(');
    const char* close_braket = find(open_braket, end(list.err_msg), ')');

    uint32_t value;
    auto [p, ec] = from_chars(open_braket + 1, close_braket, value);
    if (ec != std::errc())
    {
        throw runtime_error("failed to parse line number from: " +
                            std::string(p));
    }

    uint32_t line_num = 1;
    for (auto first = list.src.begin(), last = list.src.end(); first != last;
         first += 1, ++line_num)
    {
        auto it = find(first, last, '\n');
        os << setw(3) << right << line_num << ' '
           << string_view(first, it - first);
        if (line_num == value)
        {
            os << " <= " << list.err_msg;
        }
        else
        {
            os << '\n';
        }
        first = it;
    }
    return os;
}

static uint32_t compile_shader(std::string_view src,
                               GLenum           shader_type) noexcept(false)
{
    // create OpenGL object id for vertex shader object
    uint32_t shader = glCreateShader(shader_type);

    // load vertex shader source code into vertex_shader
    const GLchar* array_of_pointers_to_strings_with_src[1];
    array_of_pointers_to_strings_with_src[0] = src.data();

    GLint array_of_string_lengths[1];
    array_of_string_lengths[0] = static_cast<GLint>(src.size());

    glShaderSource(shader,
                   1,
                   array_of_pointers_to_strings_with_src,
                   array_of_string_lengths);

    // compile vertex shader
    glCompileShader(shader);

    // check compilation status of our shader
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (0 == success)
    {
        char info_log[1024] = { 0 };
        glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);

        std::stringstream ss;
        ss << "error: in shader:\n"
           << list_code_with_line_numbers{ src, info_log } << '\n';
        throw std::runtime_error(ss.str());
    }
    return shader;
}

void shader::create(std::string_view vertex_shader_src,
                    std::string_view geometry_shader_src,
                    std::string_view fragment_shader_src)
{

    uint32_t vertex_shader =
        compile_shader(vertex_shader_src, GL_VERTEX_SHADER);

    uint32_t geometry_shader =
        geometry_shader_src.empty()
            ? 0
            : compile_shader(geometry_shader_src, GL_GEOMETRY_SHADER);

    uint32_t fragment_shader =
        compile_shader(fragment_shader_src, GL_FRAGMENT_SHADER);

    // create complete shader program and reseive id (vertex + geometry +
    // fragment) geometry shader - will have default value
    program_id = glCreateProgram();

    glAttachShader(program_id, vertex_shader);

    if (!geometry_shader_src.empty())
    {
        glAttachShader(program_id, geometry_shader);
    }

    glAttachShader(program_id, fragment_shader);

    // now link program like in c/c++ object files
    glLinkProgram(program_id);

    int success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);

    if (0 == success)
    {
        char info_log[1024] = { 0 };
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log);

        std::stringstream ss;
        ss << "error: linking: " << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }

    // after linking shader program we don't need object parts of it
    // so we can free OpenGL memory and delete vertex and fragment parts
    glDeleteShader(vertex_shader);
    glDeleteShader(geometry_shader);
    glDeleteShader(fragment_shader);
}

static std::string read_file_content(
    const std::filesystem::path& path) noexcept(false)
{
    std::ifstream stream;
    stream.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    try
    {
        stream.open(path);
    }
    catch (...)
    {
        std::cerr << "failed to open: " << path << std::endl;
        throw;
    }
    std::stringstream ss;
    ss << stream.rdbuf();
    return ss.str();
}

shader::shader(
    const std::filesystem::path& vertex_shader_path,
    const std::filesystem::path& fragment_shader_path) noexcept(false)
    : program_id(0)
{
    std::string v_src{ read_file_content(vertex_shader_path) };
    std::string f_src{ read_file_content(fragment_shader_path) };

    create(v_src, {}, f_src);
}

shader::shader(
    const std::filesystem::path& vertex_shader_path,
    const std::filesystem::path& geometry_shader_path,
    const std::filesystem::path& fragment_shader_path) noexcept(false)
    : program_id(0)
{
    std::string v_src{ read_file_content(vertex_shader_path) };
    std::string g_src{ read_file_content(geometry_shader_path) };
    std::string f_src{ read_file_content(fragment_shader_path) };

    create(v_src, g_src, f_src);
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

void shader::bind_uniform_block(std::string_view uniform_block_name,
                                uint32_t         binding_point)
{
    uint32_t block_index =
        glGetUniformBlockIndex(program_id, uniform_block_name.data());

    glUniformBlockBinding(program_id, block_index, binding_point);
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
