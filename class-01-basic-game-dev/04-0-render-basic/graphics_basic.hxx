#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <vector>

constexpr size_t width  = 320;
constexpr size_t height = 240;

#pragma pack(push, 1)
struct color
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};
#pragma pack(pop)

constexpr size_t color_size = sizeof(color);

static_assert(3 == color_size, "24 bit per pixel(r,g,b)");

const size_t buffer_size = width * height;

extern std::array<color, buffer_size> image;

template <size_t N>
void save_image(const std::string& file_name, const std::array<color, N>& image)
{
    std::ofstream out_file;
    out_file.exceptions(std::ios_base::failbit);
    out_file.open(file_name, std::ios_base::binary);
    out_file << "P6\n" << width << ' ' << height << ' ' << 255 << '\n';
    out_file.write(reinterpret_cast<const char*>(&image), sizeof(color) * N);
}

struct position
{
    position() = default;
    position(int32_t x_, int32_t y_)
        : x(x_)
        , y(y_)
    {
    }
    int32_t x = 0;
    int32_t y = 0;
};

using pixels = std::vector<position>;

struct render
{
    virtual void   clear(color)                                   = 0;
    virtual void   set_pixel(position, color)                     = 0;
    virtual pixels pixels_positions(position start, position end) = 0;

    virtual ~render();
};

struct basic_render : render
{
    basic_render(std::array<color, buffer_size>& buffer, size_t width,
                 size_t height);

    void   clear(color) override;
    void   set_pixel(position, color) override;
    pixels pixels_positions(position start, position end) override;
    void   draw_line(position start, position end, color);

private:
    std::array<color, buffer_size>& buffer;
    const size_t                    w;
    const size_t                    h;
};

struct triangle_render : basic_render
{
    triangle_render(std::array<color, buffer_size>& buffer, size_t width,
                    size_t height);

    virtual pixels pixels_positions_triangle(position v0, position v1,
                                             position v2);
    void draw_triangles(std::vector<position>& vertexes, size_t num_vertexes,
                        color c);
};

struct triangle_indexed_render : triangle_render
{
    triangle_indexed_render(std::array<color, buffer_size>& buffer,
                            size_t width, size_t height);

    void draw_triangles(std::vector<position>& vertexes,
                        std::vector<uint8_t>& indexes, color c);
};

struct vertex
{
    float f0 = 0.f; /// x
    float f1 = 0.f; /// y
    float f2 = 0.f; /// r
    float f3 = 0.f; /// g
    float f4 = 0.f; /// b
    float f5 = 0.f; /// ?
    float f6 = 0.f; /// ?
    float f7 = 0.f; /// ?
};

float interpolate(const float f0, const float f1, const float t)
{
    assert(t >= 0.f);
    assert(t <= 1.f);
    return f0 + (f1 - f0) * t;
}

vertex interpolate(const vertex& v0, const vertex& v1, const float t)
{
    return { interpolate(v0.f0, v1.f0, t), interpolate(v0.f4, v1.f4, t),
             interpolate(v0.f1, v1.f1, t), interpolate(v0.f5, v1.f5, t),
             interpolate(v0.f2, v1.f2, t), interpolate(v0.f6, v1.f6, t),
             interpolate(v0.f3, v1.f3, t), interpolate(v0.f7, v1.f7, t) };
}

struct uniforms
{
    float f0 = 0.f;
    float f1 = 0.f;
    float f2 = 0.f;
    float f3 = 0.f;
    float f4 = 0.f;
    float f5 = 0.f;
    float f6 = 0.f;
    float f7 = 0.f;
};

struct gfx_program
{
    virtual ~gfx_program()                             = default;
    virtual void   set_uniforms(const uniforms&)       = 0;
    virtual vertex vertex_shader(const vertex& v_in)   = 0;
    virtual color  fragment_shader(const vertex& v_in) = 0;
};

struct triangle_interpolated : triangle_indexed_render
{
    triangle_interpolated(std::array<color, buffer_size>& buffer, size_t width,
                          size_t height);
    void set_gfx_program(gfx_program& program) { program_ = &program; }
    void draw_triangles(std::vector<vertex>&  vertexes,
                        std::vector<uint8_t>& indexes);

private:
    gfx_program* program_ = nullptr;
};
