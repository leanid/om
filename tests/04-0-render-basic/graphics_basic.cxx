#include "graphics_basic.hxx"

std::array<color, buffer_size> image;

render::~render() {}

basic_render::basic_render(std::array<color, buffer_size>& buffer_,
                           size_t width, size_t height)
    : buffer(buffer_)
    , w(width)
    , h(height)
{
}

void basic_render::clear(color c)
{
    std::fill(begin(buffer), end(buffer), c);
}

void basic_render::set_pixel(position p, color c)
{
    color& col = buffer.at(p.y * w + p.x);
    col        = c;
}

void basic_render::draw_line(position start, position end, color c)
{
    int x0 = start.x;
    int y0 = start.y;
    int x1 = end.x;
    int y1 = end.y;

    auto plot_line_low = [&](int x0, int y0, int x1, int y1) {
        int dx = x1 - x0;
        int dy = y1 - y0;
        int yi = 1;
        if (dy < 0)
        {
            yi = -1;
            dy = -dy;
        }
        int D = 2 * dy - dx;
        int y = y0;

        for (int x = x0; x <= x1; ++x)
        {
            set_pixel(position{ x, y }, c);
            if (D > 0)
            {
                y += yi;
                D -= 2 * dx;
            }
            D += 2 * dy;
        }
    };

    auto plot_line_high = [&](int x0, int y0, int x1, int y1) {
        int dx = x1 - x0;
        int dy = y1 - y0;
        int xi = 1;
        if (dx < 0)
        {
            xi = -1;
            dx = -dx;
        }
        int D = 2 * dx - dy;
        int x = x0;

        for (int y = y0; y <= y1; ++y)
        {
            set_pixel(position{ x, y }, c);
            if (D > 0)
            {
                x += xi;
                D -= 2 * dy;
            }
            D += 2 * dx;
        }
    };

    if (abs(y1 - y0) < abs(x1 - x0))
    {
        if (x0 > x1)
        {
            plot_line_low(x1, y1, x0, y0);
        }
        else
        {
            plot_line_low(x0, y0, x1, y1);
        }
    }
    else
    {
        if (y0 > y1)
        {
            plot_line_high(x1, y1, x0, y0);
        }
        else
        {
            plot_line_high(x0, y0, x1, y1);
        }
    }
}

int main(int, char**)
{
    const color black = { 0, 0, 0 };
    const color white = { 255, 255, 255 };
    const color green = { 0, 255, 0 };

    basic_render render(image, width, height);

    render.clear(black);

    render.draw_line(position{ 0, 0 }, position{ height - 1, height - 1 },
                     white);

    render.draw_line(position{ 0, height - 1 }, position{ height - 1, 0 },
                     green);

    for (size_t i = 0; i < 100; ++i)
    {
        position start{ static_cast<int>(rand() % width),
                        static_cast<int>(rand() % height) };
        position end{ static_cast<int>(rand() % width),
                      static_cast<int>(rand() % height) };
        color    color{ static_cast<uint8_t>(rand() % 256),
                     static_cast<uint8_t>(rand() % 256),
                     static_cast<uint8_t>(rand() % 256) };
        render.draw_line(start, end, color);
    }

    save_image("image_lines.ppm", image);
    return 0;
}
