#include "02_triangle_render.hxx"

triangle_render::triangle_render(canvas& buffer, size_t width, size_t height)
    : line_render(buffer, width, height)
{
}

pixels triangle_render::pixels_positions_triangle(position v0,
                                                  position v1,
                                                  position v2)
{
    using namespace std;
    pixels pixels_pos;

    for (auto [start, end] : { pair{ v0, v1 }, pair{ v1, v2 }, pair{ v2, v0 } })
    {
        for (auto pos : line_render::pixels_positions(start, end))
        {
            pixels_pos.push_back(pos);
        }
    }

    return pixels_pos;
}

void triangle_render::draw_triangles(std::vector<position>& vertexes,
                                     size_t                 num_vertexes,
                                     color                  c)
{
    pixels triangles_edge_pixels;

    for (size_t i = 0; i < num_vertexes / 3; ++i)
    {
        position v0 = vertexes.at(i * 3 + 0);
        position v1 = vertexes.at(i * 3 + 1);
        position v2 = vertexes.at(i * 3 + 2);

        for (auto pixel_pos : pixels_positions_triangle(v0, v1, v2))
        {
            triangles_edge_pixels.push_back(pixel_pos);
        }
    }

    // apply color to every pixel position
    for (auto pos : triangles_edge_pixels)
    {
        set_pixel(pos, c);
    }
}
