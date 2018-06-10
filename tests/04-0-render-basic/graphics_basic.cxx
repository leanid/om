#include "graphics_basic.hxx"

std::array<color, width * height> image;

int main(int, char**)
{
    color black = { 0, 0, 0 };

    for (size_t i = 0; i < width * height; ++i)
    {
        image[i] = black;
    }

    color white = { 255, 255, 255 };

    for (size_t i = 0; i < height; ++i)
    {
        image[i * width + i] = white;
    }

    save_image("image.ppm", image);
    return 0;
}
