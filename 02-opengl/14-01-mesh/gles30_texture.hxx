#pragma once

#include <filesystem>

namespace gles30
{
#ifdef _MSC_VER
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

enum class filter
{
    liner,
    nearest,
    nearest_mipmap_nearest,
    linear_mipmap_nearest,
    nearest_mipmap_linear,
    linear_mipmap_linear
};

enum class wrap
{
    repeat,
    mirrored_repeat,
    clamp_to_edge
};

class texture
{
public:
    enum class uv_type
    {
        diffuse,
        specular,
    };

    explicit texture(const fs::path& path);

    void bind();

    void generate_mipmap();

    void min_filter(const filter);
    void max_filter(const filter);

    void wrap_s(const wrap);
    void wrap_t(const wrap);

    void    set_type(const uv_type);
    uv_type get_type() const;

    ~texture();
    texture(texture&&);
    texture& operator=(texture&&);

    texture(const texture&)            = delete;
    texture& operator=(const texture&) = delete;

private:
    std::string   file_name;
    std::uint32_t texture_id;
    uv_type       texture_type = uv_type::diffuse;
};

inline void texture::set_type(const uv_type t)
{
    texture_type = t;
}

inline texture::uv_type texture::get_type() const
{
    return texture_type;
}

} // namespace gles30
