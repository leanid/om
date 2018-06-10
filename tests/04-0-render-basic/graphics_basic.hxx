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

extern std::array<color, width * height> image;

template <size_t N>
void save_image(const std::string& file_name, const std::array<color, N>& image)
{
    std::ofstream out_file;
    out_file.exceptions(std::ios_base::failbit);

    out_file.open(file_name, std::ios_base::binary);

    out_file << "P6\n" << width << ' ' << height << ' ' << 255 << '\n';
    out_file.write(reinterpret_cast<const char*>(&image), sizeof(color) * N);
}
