#include "00_canvas_basic.hxx"

int main(int, char**)
{
    const color black = { 0, 0, 0 };
    const color white = { 255, 255, 255 };
    const color green = { 0, 255, 0 };

    canvas image;

    std::fill(begin(image), end(image), green);

    image.save_image("00_green_image.ppm");

    return 0;
}
