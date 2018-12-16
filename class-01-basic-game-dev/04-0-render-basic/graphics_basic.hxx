#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
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

bool operator==(const color& l, const color& r)
{
    return l.r == r.r && l.g == r.g && l.b == r.b;
}

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

template <size_t N>
void load_image(const std::string& file_name, std::array<color, N>& image)
{
    std::ifstream in_file;
    in_file.exceptions(std::ios_base::failbit);
    in_file.open(file_name, std::ios_base::binary);
    std::string header;
    size_t      image_width  = 0;
    size_t      image_height = 0;
    std::string color_format;
    in_file >> header >> image_width >> image_height >> color_format >> std::ws;
    if (N != image_height * image_width)
    {
        throw std::runtime_error("image size not match");
    }

    in_file.read(reinterpret_cast<char*>(&image), sizeof(color) * N);
}

struct position
{
    position() = default;
    position(int32_t x_, int32_t y_)
        : x(x_)
        , y(y_)
    {
    }
    double  length() { return std::sqrt(x * x + y * y); }
    int32_t x = 0;
    int32_t y = 0;
};

position operator-(const position& left, const position& right)
{
    return { left.x - right.x, left.y - right.y };
}

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
    double f0 = 0; /// x
    double f1 = 0; /// y
    double f2 = 0; /// r
    double f3 = 0; /// g
    double f4 = 0; /// b
    double f5 = 0; /// u (texture coordinate)
    double f6 = 0; /// v (texture coordinate)
    double f7 = 0; /// ?
};

double interpolate(const double f0, const double f1, const double t)
{
    assert(t >= 0.0);
    assert(t <= 1.0);
    return f0 + (f1 - f0) * t;
}

vertex interpolate(const vertex& v0, const vertex& v1, const double t)
{
    return { interpolate(v0.f0, v1.f0, t), interpolate(v0.f1, v1.f1, t),
             interpolate(v0.f2, v1.f2, t), interpolate(v0.f3, v1.f3, t),
             interpolate(v0.f4, v1.f4, t), interpolate(v0.f5, v1.f5, t),
             interpolate(v0.f6, v1.f6, t), interpolate(v0.f7, v1.f7, t) };
}

struct uniforms
{
    double f0 = 0;
    double f1 = 0;
    double f2 = 0;
    double f3 = 0;
    double f4 = 0;
    double f5 = 0;
    double f6 = 0;
    double f7 = 0;
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
    std::vector<vertex> rasterize_triangle(const vertex& v0, const vertex& v1,
                                           const vertex& v2);
    std::vector<vertex> raster_horizontal_triangle(const vertex& single,
                                                   const vertex& left,
                                                   const vertex& right);

    void raster_one_horizontal_line(const vertex&        left_vertex,
                                    const vertex&        right_vertex,
                                    std::vector<vertex>& out);

    gfx_program* program_ = nullptr;
};
