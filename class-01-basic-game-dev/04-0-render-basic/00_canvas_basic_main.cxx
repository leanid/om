#include "00_canvas_basic.hxx"

#include <iostream>

int main(int, char**)
{
    const color green = { 0, 255, 0 };

    canvas image;

    std::fill(begin(image), end(image), green);

    const char* file_name = "00_green_image.ppm";

    image.save_image(file_name);

    canvas image_loaded;
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
