#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gles30
{

class texture;

class shader
{
public:
    shader(const std::filesystem::path& vertex_shader_path,
           const std::filesystem::path& fragment_shader_path) noexcept(false);
    shader(const std::filesystem::path& vertex_shader_path,
           const std::filesystem::path& geometry_shader_path,
           const std::filesystem::path& fragment_shader_path) noexcept(false);

    void use();

    void set_uniform(std::string_view name, bool value);
    void set_uniform(std::string_view name, std::int32_t value);
    void set_uniform(std::string_view name, float value);
    void set_uniform(std::string_view name, texture& tex, std::uint32_t index);
    void set_uniform(std::string_view name, const glm::mat4&);
    void set_uniform(std::string_view name, const glm::mat3&);
    void set_uniform(std::string_view name, const glm::vec3&);
    void set_uniform(std::string_view name, const glm::vec2&);

    void bind_uniform_block(std::string_view uniform_block_name,
                            uint32_t         binding_point);

    /// just for debug purposes you can validate current state before
    /// render geometry
    std::string validate() noexcept(false);

    shader(shader&&) noexcept;
    shader& operator=(shader&&) noexcept;
    ~shader() noexcept;

    shader(const shader&)            = delete;
    shader& operator=(const shader&) = delete;

private:
    void          create(std::string_view vertex_shader_src,
                         std::string_view geometry_shader_src,
                         std::string_view fragment_shader_src) noexcept(false);
    std::uint32_t program_id;
};
} // end namespace gles30
