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
    uint8_t     r = 0;
    uint8_t     g = 0;
    uint8_t     b = 0;
    friend bool operator==(const color& l, const color& r);
};
#pragma pack(pop)

const size_t buffer_size = width * height;

class canvas : public std::array<color, buffer_size>
{
public:
    /// PPM - image format:
    /// https://ru.wikipedia.org/wiki/Portable_anymap#%D0%9F%D1%80%D0%B8%D0%BC%D0%B5%D1%80_PPM
    void save_image(const std::string& file_name)
    {
        std::ofstream out_file;
        out_file.exceptions(std::ios_base::failbit);
        out_file.open(file_name, std::ios_base::binary);
        out_file << "P6\n" << width << ' ' << height << ' ' << 255 << '\n';
        std::streamsize buf_size =
            static_cast<std::streamsize>(sizeof(color) * size());
        out_file.write(reinterpret_cast<const char*>(this), buf_size);
    }

    void load_image(const std::string& file_name)
    {
        std::ifstream in_file;
        in_file.exceptions(std::ios_base::failbit);
        in_file.open(file_name, std::ios_base::binary);
        std::string header;
        size_t      image_width  = 0;
        size_t      image_height = 0;
        std::string color_format;
        in_file >> header >> image_width >> image_height >> color_format >>
            std::ws;
        if (size() != image_height * image_width)
        {
            throw std::runtime_error("image size not match");
        }
        std::streamsize buf_size =
            static_cast<std::streamsize>(sizeof(color) * size());
        in_file.read(reinterpret_cast<char*>(this), buf_size);
    }
};

struct position
{
    double          length() { return std::sqrt(x * x + y * y); }
    friend position operator-(const position& left, const position& right);
    friend bool     operator==(const position& left, const position& right);
    int32_t         x = 0;
    int32_t         y = 0;
};

using pixels = std::vector<position>;

struct irender
{
    virtual void   clear(color)                                   = 0;
    virtual void   set_pixel(position, color)                     = 0;
    virtual pixels pixels_positions(position start, position end) = 0;

    virtual ~irender();
};