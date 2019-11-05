#include "01_line_render.hxx"

int main(int, char**)
{
    const color black = { 0, 0, 0 };
    const color white = { 255, 255, 255 };
    const color green = { 0, 255, 0 };

    canvas image;

    line_render render(image, width, height);

    render.clear(black);

    render.draw_line(position{ 0, 0 }, position{ height - 1, height - 1 },
                     white);

    render.draw_line(position{ 0, height - 1 }, position{ height - 1, 0 },
                     green);

    for (size_t i = 0; i < 100; ++i)
    {
        position start{ rand() % static_cast<int>(width),
                        rand() % static_cast<int>(height) };
        position end{ rand() % static_cast<int>(width),
                      rand() % static_cast<int>(height) };
        color    color{ static_cast<uint8_t>(rand() % 256),
                     static_cast<uint8_t>(rand() % 256),
                     static_cast<uint8_t>(rand() % 256) };
        render.draw_line(start, end, color);
    }

    image.save_image("01_lines.ppm");
    return 0;
}
