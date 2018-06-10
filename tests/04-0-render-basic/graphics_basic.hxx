#pragma once

#include <array>
#include <cstddef>
#include <fstream>

// TODO add render functions

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
    size_t x;
    size_t y;
};

struct render
{
    virtual void clear(color)                                   = 0;
    virtual void set_pixel(position, color)                     = 0;
    virtual void draw_line(position start, position end, color) = 0;

    virtual ~render();
};

struct basic_render : render
{
    basic_render(std::array<color, buffer_size>& buffer, size_t width,
                 size_t height);

    void clear(color) override;
    void set_pixel(position, color) override;
    void draw_line(position start, position end, color) override;

private:
    std::array<color, buffer_size>& buffer;
    const size_t                    w;
    const size_t                    h;
};
