#include "03_triangle_indexed_render.hxx"

// NOLINTNEXTLINE
int main(int, char**)
{
    constexpr color black = { .r = 0, .g = 0, .b = 0 };
    constexpr color green = { .r = 0, .g = 255, .b = 0 };

    constexpr size_t width  = 320;
    constexpr size_t height = 240;

    canvas image(width, height);

    std::vector<position> vertex_buffer;

    constexpr size_t max_x = 10;
    constexpr size_t max_y = 10;

    constexpr int32_t step_x = (width - 1) / max_x;
    constexpr int32_t step_y = (height - 1) / max_y;

    for (size_t i = 0; i <= max_y; ++i)
    {
        for (size_t j = 0; j <= max_x; ++j)
        {
            position v{ .x = static_cast<int>(j) * step_x,
                        .y = static_cast<int>(i) * step_y };

            vertex_buffer.push_back(v);
        }
    }

    assert(vertex_buffer.size() == (max_x + 1) * (max_y + 1));

    std::vector<uint8_t> index_buffer;

    for (size_t x = 0; x < max_x; ++x)
    {
        for (size_t y = 0; y < max_y; ++y)
        {
            auto    index0 = static_cast<uint8_t>(y * (max_y + 1) + x);
            auto    index1 = static_cast<uint8_t>(index0 + (max_y + 1) + 1);
            uint8_t index2 = index1 - 1;
            uint8_t index3 = index0 + 1;

            index_buffer.push_back(index0);
            index_buffer.push_back(index1);
            index_buffer.push_back(index2);

            index_buffer.push_back(index0);
            index_buffer.push_back(index3);
            index_buffer.push_back(index1);
        }
    }

    triangle_indexed_render indexed_render(image, width, height);
    indexed_render.clear(black);

    indexed_render.draw_triangles(vertex_buffer, index_buffer, green);

    image.save_image("04_triangles_indexes.ppm");

    canvas tex_image(0, 0);
    tex_image.load_image("04_triangles_indexes.ppm");

    assert(image == tex_image);

    return 0;
}
