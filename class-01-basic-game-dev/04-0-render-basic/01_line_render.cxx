#include "01_line_render.hxx"

#include <algorithm>
#include <cassert>

line_render::line_render(canvas& buffer_, size_t width, size_t height)
    : buffer(buffer_)
    , w(width)
    , h(height)
{
}

void line_render::clear(color c)
{
    std::fill(begin(buffer), end(buffer), c);
}

void line_render::set_pixel(position p, color c)
{
    const size_t i =
        static_cast<unsigned>(p.y) * w + static_cast<unsigned>(p.x);
    color& col = buffer.at(i);
    col        = c;
}

pixels line_render::pixels_positions(position start, position end)
{
    pixels result;
    int    x0 = start.x;
    int    y0 = start.y;
    int    x1 = end.x;
    int    y1 = end.y;

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
            result.push_back(position{ x, y });
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
            result.push_back(position{ x, y });
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
    return result;
}

void line_render::draw_line(position start, position end, color c)
{
    pixels l = pixels_positions(start, end);
    std::for_each(l.begin(), l.end(), [&](auto& pos) { set_pixel(pos, c); });
}
