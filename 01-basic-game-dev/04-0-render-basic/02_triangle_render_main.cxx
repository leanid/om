#include "02_triangle_render.hxx"

int main(int, char**)
{
    const color black = { 0, 0, 0 };
    const color green = { 0, 255, 0 };

    size_t width  = 320;
    size_t height = 240;

    canvas image(width, height);

    triangle_render render_tri(image, width, height);
    render_tri.clear(black);

    std::vector<position> triangle;
    triangle.push_back(position{ 0, 0 });
    triangle.push_back(
        position{ static_cast<int>(width - 1), static_cast<int>(height - 1) });
    triangle.push_back(position{ 0, static_cast<int>(height - 1) });

    render_tri.draw_triangles(triangle, 3, green);

    image.save_image("02_triangle.ppm");

    render_tri.clear(black);

    size_t max_x = 10;
    size_t max_y = 10;

    std::vector<position> triangles;

    for (size_t i = 0; i < max_x; ++i)
    {
        for (size_t j = 0; j < max_y; ++j)
        {
            int32_t step_x = (width - 1) / max_x;
            int32_t step_y = (height - 1) / max_y;

            position v0{ 0 + static_cast<int>(i) * step_x,
                         0 + static_cast<int>(j) * step_y };
            position v1{ v0.x + step_x, v0.y + step_y };
            position v2{ v0.x, v0.y + step_y };
            position v3{ v0.x + step_x, v0.y };

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
