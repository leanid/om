#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gles30
{

#ifdef _MSC_VER
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

class texture;

class shader
{
public:
    shader(std::string_view vertex_shader_src,
           std::string_view fragment_shader_src) noexcept(false);
    shader(const fs::path& vertex_shader_path,
           const fs::path& fragment_shader_path) noexcept(false);

    void use();

    void set_uniform(std::string_view name, bool value);
    void set_uniform(std::string_view name, std::int32_t value);
    void set_uniform(std::string_view name, float value);
    void set_uniform(std::string_view name, texture& tex, std::uint32_t index);
    void set_uniform(std::string_view name, const glm::mat4&);
    void set_uniform(std::string_view name, const glm::mat3&);

    /// just for debug purposes you can validate current state before
    /// render geometry
    std::string validate() noexcept(false);

    shader(shader&&) noexcept;
    shader& operator=(shader&&) noexcept;
    ~shader() noexcept;

    shader(const shader&) = delete;
    shader& operator=(const shader&) = delete;

private:
    std::uint32_t program_id;
};
} // end namespace gles30
