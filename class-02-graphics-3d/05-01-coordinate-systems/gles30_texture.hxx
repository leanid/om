#pragma once

#include <filesystem>

namespace gles30
{

namespace fs = std::experimental::filesystem;

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
    explicit texture(const fs::path& path);

    void bind();

    void generate_mipmap();

    void min_filter(const filter);
    void max_filter(const filter);

    void wrap_s(const wrap);
    void wrap_t(const wrap);

    ~texture();
    texture(texture&&);
    texture& operator=(texture&&);

    texture(const texture&) = delete;
    texture& operator=(const texture&) = delete;

private:
    std::uint32_t texture_id;
};
} // namespace gles30
