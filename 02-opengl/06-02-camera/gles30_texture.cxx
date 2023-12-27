#include "gles30_texture.hxx"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma GCC diagnostic pop

#include "opengles30.hxx"

namespace gles30
{
texture::texture(const fs::path& path)
    : texture_id{ 0 }
{
    stbi_set_flip_vertically_on_load(1);

    int width;
    int height;
    int channels;

    const int prefered_channels_count = 4;

    std::unique_ptr<uint8_t, void (*)(void*)> data(
        stbi_load(path.u8string().c_str(),
                  &width,
                  &height,
                  &channels,
                  prefered_channels_count),
        &stbi_image_free);

    if (data.get() == nullptr)
    {
        throw std::runtime_error("can't create texture from file: " +
                                 path.u8string());
    }

    glGenTextures(1, &texture_id);
    gl_check();

    glBindTexture(GL_TEXTURE_2D, texture_id);
    gl_check();

    wrap_s(wrap::repeat);
    wrap_t(wrap::repeat);

    max_filter(filter::liner);
    min_filter(filter::linear_mipmap_linear);

    GLint mipmap_level = 0;
    GLint border       = 0;
    if (3 == prefered_channels_count)
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
        gl_check();
    }
    else if (4 == prefered_channels_count)
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
        gl_check();
    }
    else
    {
        throw std::runtime_error("3 or 4 channel textures only now supported");
    }

    generate_mipmap();
}

void texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, texture_id);
    gl_check();
}

void texture::generate_mipmap()
{
    glGenerateMipmap(GL_TEXTURE_2D);
    gl_check();
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
    gl_check();
}

void texture::max_filter(const filter value)
{
    GLint gl_filtering = to_gl_enum(value);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filtering);
    gl_check();
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
    gl_check();
}

void texture::wrap_t(const wrap value)
{
    GLint gl_wrap = to_gl_enum(value);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap);
    gl_check();
}

texture::~texture()
{
    glDeleteTextures(1, &texture_id);
    gl_check();
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
