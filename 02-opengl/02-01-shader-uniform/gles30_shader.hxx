#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace gles30
{
class shader
{
public:
    shader(std::string_view vertex_shader_src,
           std::string_view fragment_shader_src) noexcept(false);
    shader(const std::filesystem::path& vertex_shader_path,
           const std::filesystem::path& fragment_shader_path) noexcept(false);

    void use();

    void set_uniform(std::string_view name, bool value);
    void set_uniform(std::string_view name, std::int32_t value);
    void set_uniform(std::string_view name, float value);

    /// just for debug purposes you can validate current state before
    /// render geometry
    std::string validate() noexcept(false);

    shader(shader&&) noexcept;
    shader& operator=(shader&&) noexcept;
    ~shader() noexcept;

    shader(const shader&)            = delete;
    shader& operator=(const shader&) = delete;

private:
    std::uint32_t program_id;
};
} // end namespace gles30
