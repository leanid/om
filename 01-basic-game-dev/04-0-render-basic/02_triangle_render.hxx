#pragma once

#include "01_line_render.hxx"

struct triangle_render : line_render
{
    triangle_render(canvas& buffer, size_t width, size_t height);

    virtual pixels pixels_positions_triangle(position v0, position v1,
                                             position v2);
    void draw_triangles(std::vector<position>& vertexes, size_t num_vertexes,
                        color c);
};
