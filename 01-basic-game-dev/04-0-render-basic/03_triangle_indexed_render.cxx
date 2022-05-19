#include "03_triangle_indexed_render.hxx"

triangle_indexed_render::triangle_indexed_render(canvas& buffer, size_t width,
                                                 size_t height)
    : triangle_render(buffer, width, height)
{
}

void triangle_indexed_render::draw_triangles(std::vector<position>& vertexes,
                                             std::vector<uint8_t>&  indexes,
                                             color                  c)
{
    pixels triangles_edge_pixels;

    for (size_t i = 0; i < indexes.size() / 3; ++i)
    {
        uint8_t index0 = indexes[i * 3 + 0];
        uint8_t index1 = indexes[i * 3 + 1];
        uint8_t index2 = indexes[i * 3 + 2];

        position v0 = vertexes.at(index0);
        position v1 = vertexes.at(index1);
        position v2 = vertexes.at(index2);

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
