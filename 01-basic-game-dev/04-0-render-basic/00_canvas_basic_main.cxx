#include "00_canvas_basic.hxx"

#include <algorithm>
#include <iostream>

int main(int, char**)
{
    const color green = { .r=0, .g=255, .b=0 };

    size_t width  = 320;
    size_t height = 240;

    canvas image(width, height);

    std::ranges::fill(image, green);

    const char* file_name = "00_green_image.ppm";

    image.save_image(file_name);

    canvas image_loaded(0, 0);
    image_loaded.load_image(file_name);

    if (image != image_loaded)
    {
        std::cerr << "image != image_loaded\n";
        return 1;
    }
    else
    {
        std::cout << "image == image_loaded\n";
    }

    return 0;
}
