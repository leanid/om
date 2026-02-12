#include "gles30_texture.hxx"

#include <algorithm>
#include <array>
#include <numeric>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#pragma GCC diagnostic pop

#include "opengles30.hxx"

namespace gles30
{
texture::texture(const type tex_type, size_t width, size_t height)
    : file_name{ "from memory" }
    , texture_id{ 0 }
    , texture_type{ tex_type }
{
    gen_texture_set_filters_and_wrap();

    GLint mipmap_level = 0;
    GLint border       = 0;
    // allocate memory for texture
    glTexImage2D(GL_TEXTURE_2D,
                 mipmap_level,
                 GL_RGB,
                 width,
                 height,
                 border,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 nullptr);
}

void texture::set_default_wrap_and_filters()
{
    wrap_s(wrap::repeat);
    wrap_t(wrap::repeat);

    // if you plan to use this texture in framebuffer object
    // do not set nothing else filter::liner (no min/mag mitmap can be used)
    max_filter(filter::liner);
    min_filter(filter::liner);
}

void texture::gen_texture_set_filters_and_wrap()
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    set_default_wrap_and_filters();
}

texture::texture(const std::filesystem::path& path,
                 const type                   tex_type,
                 const opt                    options)
    : file_name{ path.u8string() }
    , texture_id{ 0 }
    , texture_type{ tex_type }
{
    if (options == opt::flip_y)
    {
        stbi_set_flip_vertically_on_load(1);
    }

    int width;
    int height;
    int channels;

    const int prefered_channels_count = 0; // same as in texture

    std::unique_ptr<uint8_t, void (*)(void*)> data(
        stbi_load(file_name.c_str(),
                  &width,
                  &height,
                  &channels,
                  prefered_channels_count),
        &stbi_image_free);

    if (data.get() == nullptr)
    {
        throw std::runtime_error("can't create texture from file: " +
                                 file_name);
    }

    gen_texture_set_filters_and_wrap();

    GLint mipmap_level = 0;
    GLint border       = 0;
    if (3 == channels)
    {
        glTexImage2D(GL_TEXTURE_2D,
                     mipmap_level,
                     GL_RGB,
                     width,
                     height,
                     border,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     data.get());
    }
    else if (4 == channels)
    {
        glTexImage2D(GL_TEXTURE_2D,
                     mipmap_level,
                     GL_RGBA,
                     width,
                     height,
                     border,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     data.get());

        // RBG + A - should be clamped on border color to border not
        // reverse side pixel (or leave 1px on boarder on texture)
        wrap_s(wrap::clamp_to_edge);
        wrap_t(wrap::clamp_to_edge);
    }
    else
    {
        throw std::runtime_error("3 or 4 channel textures only now supported");
    }

    generate_mipmap();
}

static std::string join_strings_with_spaces(
    const std::array<std::filesystem::path, 6>& faces)
{
    std::string result;
    std::accumulate(begin(faces),
                    end(faces),
                    result,
                    [](std::string& result, const std::filesystem::path& p)
                    {
                        if (!result.empty())
                        {
                            result.push_back(' ');
                        }
                        return result += p.string();
                    });
    return result;
}

texture::texture(const std::array<std::filesystem::path, 6>& faces,
                 const opt                                   options)
    : file_name{ join_strings_with_spaces(faces) }
    , texture_id{}
    , texture_type{ type::cubemap }
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    const std::array<int, 6> face_type{
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    if (opt::flip_y == options)
    {
        stbi_set_flip_vertically_on_load(1);
    }

    for (size_t i = 0; i < faces.size(); ++i)
    {
        const auto& path = faces.at(i).u8string();
        const auto& type = face_type.at(i);
        int32_t     width{};
        int32_t     height{};
        int32_t     num_channels{};
        const int   prefered_channels_count = 0; // same as in texture
        std::unique_ptr<unsigned char, void (*)(void*)> data{
            stbi_load(path.c_str(),
                      &width,
                      &height,
                      &num_channels,
                      prefered_channels_count),
            stbi_image_free
        };
        if (!data)
        {
            throw std::runtime_error("error: loading cubemap failed: " + path);
        }
        const int32_t mipmap_level = 0;
        const int32_t boarder      = 0;
        glTexImage2D(type,
                     mipmap_level,
                     GL_RGB,
                     width,
                     height,
                     boarder,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     data.get());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void texture::bind()
{
    if (type::cubemap == texture_type)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
}

void texture::generate_mipmap()
{
    glGenerateMipmap(GL_TEXTURE_2D);
}

static int to_gl_enum(const filter value)
{
    switch (value)
    {
        case filter::nearest:
            return GL_NEAREST;
        case filter::liner:
            return GL_LINEAR;
        case filter::nearest_mipmap_nearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        case filter::nearest_mipmap_linear:
            return GL_NEAREST_MIPMAP_LINEAR;
        case filter::linear_mipmap_nearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        case filter::linear_mipmap_linear:
            return GL_LINEAR_MIPMAP_LINEAR;
    }
    throw std::runtime_error("bad filter value: " +
                             std::to_string(static_cast<int>(value)));
}

void texture::min_filter(const filter value)
{
    GLint gl_filtering = to_gl_enum(value);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filtering);
}

void texture::max_filter(const filter value)
{
    GLint gl_filtering = to_gl_enum(value);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filtering);
}

static int to_gl_enum(const wrap value)
{
    switch (value)
    {
        case wrap::repeat:
            return GL_REPEAT;
        case wrap::mirrored_repeat:
            return GL_MIRRORED_REPEAT;
        case wrap::clamp_to_edge:
            return GL_CLAMP_TO_EDGE;
    }
    throw std::runtime_error("bad wrap value: " +
                             std::to_string(static_cast<int>(value)));
}

void texture::wrap_s(const wrap value)
{
    GLint gl_wrap = to_gl_enum(value);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap);
}

void texture::wrap_t(const wrap value)
{
    GLint gl_wrap = to_gl_enum(value);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap);
}

texture::~texture()
{
    glDeleteTextures(1, &texture_id);
}
texture::texture(texture&& other)
    : texture_id{ other.texture_id }
{
    other.texture_id = 0;
}
texture& texture::operator=(texture&& other)
{
    texture tmp(std::move(other));
    std::swap(tmp.texture_id, texture_id);
    return *this;
}
} // namespace gles30
