#pragma once

#include <filesystem>

#include "opengles30.hxx"

namespace gles30
{

enum class wrap
{
    repeat,
    mirrored_repeat,
    clamp_to_edge
};

class texture
{
public:
    struct extent
    {
        size_t width;
        size_t height;
    };

    enum class type
    {
        diffuse,
        specular,
        cubemap,
        multisample2d
    };

    enum class opt
    {
        no_flip,
        flip_y
    };

    /// type in {diffuse, specular}
    texture(const type, const extent size);
    /// type in {multisample2d}
    texture(const extent size, const size_t num_of_samples);
    /// type in {diffuse, specular}
    texture(const std::filesystem::path& path,
            const type,
            const opt = opt::no_flip);
    /// type in {cubemap}
    texture(const std::array<std::filesystem::path, 6>& faces,
            const opt = opt::no_flip);

    void bind();

    void bind_to_framebuffer();

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
    void gen_texture_and_bind_it();
    void gen_texture_set_filters_and_wrap();
    void set_default_wrap_and_filters();
    void throw_exception_if_not_diffuse_or_specular();
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
