#pragma once

#include <string_view>

namespace gles30
{
class shader
{
public:
    shader(std::string_view vertex_shader_src,
           std::string_view fragment_shader_src) noexcept(false);

    void use();

    void set_uniform(std::string_view name, bool value);
    void set_uniform(std::string_view name, std::int32_t value);
    void set_uniform(std::string_view name, float value);

    /// just for debug purposes you can validate current state before
    /// render geometry
    void validate() noexcept(false);

    shader(shader&&) noexcept;
    shader& operator=(shader&&) noexcept;
    ~shader() noexcept;

    shader(const shader&) = delete;
    shader& operator=(const shader&) = delete;

private:
    std::uint32_t program_id;
};
} // end namespace gles30
