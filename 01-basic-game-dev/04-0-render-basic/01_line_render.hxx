#pragma once

#include "00_canvas_basic.hxx"

struct line_render : irender
{
    line_render(canvas& buffer, size_t width, size_t height);

    void   clear(color) override;
    void   set_pixel(position, color) override;
    pixels pixels_positions(position start, position end) override;
    void   draw_line(position start, position end, color);

private:
    canvas&      buffer;
    const size_t w;
    const size_t h;
};
