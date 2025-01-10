#include "02_triangle_render.hxx"
// NOLINTNEXTLINE
int main(int, char**)
{
    const color black = { .r = 0, .g = 0, .b = 0 };
    const color green = { .r = 0, .g = 255, .b = 0 };

    constexpr size_t width  = 320;
    constexpr size_t height = 240;

    canvas image(width, height);

    triangle_render render_tri(image, width, height);
    render_tri.clear(black);

    std::vector<position> triangle_vertices;
    triangle_vertices.push_back(position{ .x = 0, .y = 0 });
    triangle_vertices.push_back(position{ .x = static_cast<int>(width - 1),
                                          .y = static_cast<int>(height - 1) });
    triangle_vertices.push_back(
        position{ .x = 0, .y = static_cast<int>(height - 1) });

    render_tri.draw_triangles(triangle_vertices, 3, green);

    image.save_image("02_triangle.ppm");

    render_tri.clear(black);

    constexpr size_t  max_x  = 10;
    constexpr size_t  max_y  = 10;
    constexpr int32_t step_x = (width - 1) / max_x;
    constexpr int32_t step_y = (height - 1) / max_y;

    std::vector<position> triangles;
    triangles.reserve(max_x * max_y * 3 * 2);

    for (size_t i = 0; i < max_x; ++i)
    {
        for (size_t j = 0; j < max_y; ++j)
        {
            position v0{ .x = 0 + static_cast<int>(i) * step_x,
                         .y = 0 + static_cast<int>(j) * step_y };
            position v1{ .x = v0.x + step_x, .y = v0.y + step_y };
            position v2{ .x = v0.x, .y = v0.y + step_y };
            position v3{ .x = v0.x + step_x, .y = v0.y };

            triangles.push_back(v0);
            triangles.push_back(v1);
            triangles.push_back(v2);

            triangles.push_back(v0);
            triangles.push_back(v3);
            triangles.push_back(v1);
        }
    }

    render_tri.draw_triangles(triangles, triangles.size(), green);

    image.save_image("03_triangles.ppm");

    return 0;
}
