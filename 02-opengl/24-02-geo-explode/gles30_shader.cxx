#include "gles30_shader.hxx"
#include <array>

#include <algorithm>
#include <cassert>
#include <ranges>
#include <charconv>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
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

    auto       open_it     = std::ranges::find(list.err_msg, '(');
    const char* open_braket = (open_it != list.err_msg.end()) ? &*open_it : list.err_msg.data();
    auto close_range = std::string_view(open_braket, list.err_msg.data() + list.err_msg.size());
    auto       close_it    = std::ranges::find(close_range, ')');
    const char* close_braket = (close_it != close_range.end()) ? &*close_it : close_range.data() + close_range.size();

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
        auto it = std::ranges::find(std::ranges::subrange(first, last), '\n');
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
    std::array<const GLchar*, 1> array_of_pointers_to_strings_with_src{};
    array_of_pointers_to_strings_with_src[0] = src.data();

    std::array<GLint, 1> array_of_string_lengths{};
    array_of_string_lengths[0] = static_cast<GLint>(src.size());

    glShaderSource(shader,
                   1,
                   array_of_pointers_to_strings_with_src.data(),
                   array_of_string_lengths.data());

    // compile vertex shader
    glCompileShader(shader);

    // check compilation status of our shader
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (0 == success)
    {
        std::array<char, 1024> info_log{};
        glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log.data());

        std::stringstream ss;
        ss << "error: in shader:\n"
           << list_code_with_line_numbers{ .src = src,
                                           .err_msg = std::string_view(info_log.data()) } << '\n';
        throw std::runtime_error(ss.str());
    }
    return shader;
}

void shader::create(shader_sources src)
{
    uint32_t vertex_shader =
        compile_shader(src.vertex, GL_VERTEX_SHADER);

    uint32_t geometry_shader =
        src.geometry.empty()
            ? 0
            : compile_shader(src.geometry, GL_GEOMETRY_SHADER);

    uint32_t fragment_shader =
        compile_shader(src.fragment, GL_FRAGMENT_SHADER);

    // create complete shader program and reseive id (vertex + geometry +
    // fragment) geometry shader - will have default value
    program_id = glCreateProgram();

    glAttachShader(program_id, vertex_shader);

    if (!src.geometry.empty())
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
        std::array<char, 1024> info_log{};
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log.data());

        std::stringstream ss;
        ss << "error: linking: " << info_log.data() << std::endl;
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

    create({ .vertex = v_src, .geometry = {}, .fragment = f_src });
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

    create({ .vertex = v_src, .geometry = g_src, .fragment = f_src });
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
    const std::string uniform_name(name);
    GLint uniform_index = glGetUniformLocation(program_id, uniform_name.c_str());

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
    std::string block_name(uniform_block_name);
    uint32_t    block_index =
        glGetUniformBlockIndex(program_id, block_name.c_str());

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
        std::array<char, 4096> info_log{};
        glGetProgramInfoLog(program_id, sizeof(info_log), nullptr, info_log.data());

        if (strlen(info_log.data()) > 0)
        {
            return { info_log.data() };
        }
    }
    else
    {
        throw std::runtime_error("can't get validation status");
    }
    return {};
}
} // namespace gles30
