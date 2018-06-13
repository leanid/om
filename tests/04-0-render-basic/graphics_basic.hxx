#pragma once

#include <array>
#include <cstddef>
#include <fstream>
#include <vector>

constexpr size_t width  = 320;
constexpr size_t height = 240;

#pragma pack(push, 1)
struct color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
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

using line = std::vector<position>;

struct render
{
    virtual void clear(color)                                 = 0;
    virtual void set_pixel(position, color)                   = 0;
    virtual line line_positions(position start, position end) = 0;

    virtual ~render();
};

struct basic_render : render
{
    basic_render(std::array<color, buffer_size>& buffer, size_t width,
                 size_t height);

    void clear(color) override;
    void set_pixel(position, color) override;
    line line_positions(position start, position end) override;
    void draw_line(position start, position end, color);

private:
    std::array<color, buffer_size>& buffer;
    const size_t                    w;
    const size_t                    h;
};

struct triangle_render : basic_render
{
    triangle_render(std::array<color, buffer_size>& buffer, size_t width,
                    size_t height);

    void draw_triangles(std::vector<position>& vertexes, size_t num_vertexes,
                        color);
};
