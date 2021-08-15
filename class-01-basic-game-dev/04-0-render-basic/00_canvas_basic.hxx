#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <vector>

#pragma pack(push, 1)
struct color
{
    uint8_t     r = 0;
    uint8_t     g = 0;
    uint8_t     b = 0;
    friend bool operator==(const color& l, const color& r);
};
#pragma pack(pop)

constexpr color color_red{ 255, 0, 0 };
constexpr color color_green{ 0, 255, 0 };
constexpr color color_blue{ 0, 0, 255 };

class canvas
{
public:
    canvas(size_t w, size_t h)
        : width{ w }
        , height{ h }
    {
        pixels.resize(width * height);
    }
    /// PPM - image format:
    /// https://ru.wikipedia.org/wiki/Portable_anymap#%D0%9F%D1%80%D0%B8%D0%BC%D0%B5%D1%80_PPM
    void save_image(const std::string& file_name)
    {
        std::ofstream out_file;
        out_file.exceptions(std::ios_base::failbit);
        out_file.open(file_name, std::ios_base::binary);
        // clang-format off
        out_file << "P6\n"
                 << width << ' ' << height << ' ' << 255 << '\n';
        // clang-format on
        std::streamsize buf_size =
            static_cast<std::streamsize>(sizeof(color) * pixels.size());
        out_file.write(reinterpret_cast<const char*>(pixels.data()), buf_size);
    }

    void load_image(const std::string& file_name)
    {
        std::ifstream in_file;
        in_file.exceptions(std::ios_base::failbit);
        in_file.open(file_name, std::ios_base::binary);
        std::string header;
        std::string color_format;
        char        last_next_line = 0;
        // clang-format off
        in_file >> header
                >> width
                >> height
                >> color_format
                >> std::noskipws
                >> last_next_line;
        // clang-format on
        // in_file.read(&last_next_line, 1);

        if (!iswspace(last_next_line))
        {
            throw std::runtime_error("expected whitespace");
        }

        pixels.resize(width * height);

        if (pixels.size() != width * height)
        {
            throw std::runtime_error("image size not match");
        }
        std::streamsize buf_size =
            static_cast<std::streamsize>(sizeof(color) * pixels.size());
        in_file.read(reinterpret_cast<char*>(pixels.data()), buf_size);
    }

    void set_pixel(size_t x, size_t y, color col)
    {
        const size_t liner_index_in_buffer = width * y + x;
        color&       target_pixel          = pixels.at(liner_index_in_buffer);
        target_pixel                       = col;
    }

    color get_pixel(size_t x, size_t y) const
    {
        const size_t liner_index_in_buffer = width * y + x;
        return pixels.at(liner_index_in_buffer);
    }

    bool operator==(const canvas& other) const
    {
        return pixels == other.pixels;
    }

    bool operator!=(const canvas& other) const { return !(*this == other); }

    auto begin() { return pixels.begin(); }
    auto end() { return pixels.end(); }

    std::vector<color>& get_pixels() { return pixels; }

    size_t get_width() const { return width; }
    size_t get_height() const { return height; }

private:
    size_t             width  = 0;
    size_t             height = 0;
    std::vector<color> pixels; // same: color pixels[buffer_size];
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
