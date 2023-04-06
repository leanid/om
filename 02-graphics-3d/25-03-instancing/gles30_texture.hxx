#pragma once

#include <filesystem>

namespace gles30
{

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
    enum class type
    {
        diffuse,
        specular,
        cubemap
    };

    enum class opt
    {
        no_flip,
        flip_y
    };

    texture(const type, size_t width, size_t height);
    texture(const std::filesystem::path& path,
            const type,
            const opt = opt::no_flip);
    texture(const std::array<std::filesystem::path, 6>& faces,
            const opt = opt::no_flip);

    void bind();

    void generate_mipmap();

    void min_filter(const filter);
    void max_filter(const filter);

    void wrap_s(const wrap);
    void wrap_t(const wrap);

    void set_type(const type);
    type get_type() const;

    ~texture();
    texture(texture&&);
    texture& operator=(texture&&);

    texture(const texture&)            = delete;
    texture& operator=(const texture&) = delete;

private:
    void gen_texture_set_filters_and_wrap();
    void set_default_wrap_and_filters();
    friend class framebuffer;

    std::string   file_name;
    std::uint32_t texture_id;
    type          texture_type = type::diffuse;
};

inline void texture::set_type(const type t)
{
    texture_type = t;
}

inline texture::type texture::get_type() const
{
    return texture_type;
}

} // namespace gles30
