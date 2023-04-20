#include "00_canvas_basic.hxx"
#include "01_line_render.hxx"

int main(int, char**)
{
    const color black = { 0, 0, 0 };
    const color white = { 255, 255, 255 };
    const color green = { 0, 255, 0 };

    int32_t width  = 320;
    int32_t height = 240;

    canvas image(width, height);

    line_render render(image, width, height);

    render.clear(black);

    render.draw_line(
        position{ 0, 0 }, position{ height - 1, height - 1 }, white);

    render.draw_line(
        position{ 0, height - 1 }, position{ height - 1, 0 }, green);

    for (size_t i = 0; i < 100; ++i)
    {
        position start{ position::generate_random(width, height) };
        position end{ position::generate_random(width, height) };
        color    color{ static_cast<uint8_t>(rand() % 256),
                     static_cast<uint8_t>(rand() % 256),
                     static_cast<uint8_t>(rand() % 256) };
        render.draw_line(start, end, color);
    }

    image.save_image("01_lines.ppm");
    return 0;
}
