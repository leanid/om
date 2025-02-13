#include "00_canvas_basic.hxx"
#include "01_line_render.hxx"
// NOLINTNEXTLINE
int main(int, char**)
{
    const color black = { .r = 0, .g = 0, .b = 0 };
    const color white = { .r = 255, .g = 255, .b = 255 };
    const color green = { .r = 0, .g = 255, .b = 0 };

    int32_t width  = 320;
    int32_t height = 240;

    canvas image(width, height);

    line_render render(image, width, height);

    render.clear(black);

    render.draw_line(position{ .x = 0, .y = 0 },
                     position{ .x = height - 1, .y = height - 1 },
                     white);

    render.draw_line(position{ .x = 0, .y = height - 1 },
                     position{ .x = height - 1, .y = 0 },
                     green);

    for (size_t i = 0; i < 100; ++i)
    {
        position start{ position::generate_random(width, height) };
        position end{ position::generate_random(width, height) };
        color    color{ .r = static_cast<uint8_t>(rand() % 256),
                        .g = static_cast<uint8_t>(rand() % 256),
                        .b = static_cast<uint8_t>(rand() % 256) };
        render.draw_line(start, end, color);
    }

    image.save_image("01_lines.ppm");
    return 0;
}
