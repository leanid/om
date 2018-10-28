#include "graphics_basic.hxx"

#include <algorithm>
#include <cassert>

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

pixels basic_render::pixels_positions(position start, position end)
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

void basic_render::draw_line(position start, position end, color c)
{
    pixels l = pixels_positions(start, end);
    std::for_each(l.begin(), l.end(), [&](auto& pos) { set_pixel(pos, c); });
}

triangle_render::triangle_render(std::array<color, buffer_size>& buffer,
                                 size_t width, size_t height)
    : basic_render(buffer, width, height)
{
}

pixels triangle_render::pixels_positions_triangle(position v0, position v1,
                                                  position v2)
{
    using namespace std;
    pixels pixels_pos;

    for (auto [start, end] : { pair{ v0, v1 }, pair{ v1, v2 }, pair{ v2, v0 } })
    {
        for (auto pos : basic_render::pixels_positions(start, end))
        {
            pixels_pos.push_back(pos);
        }
    }

    return pixels_pos;
}

void triangle_render::draw_triangles(std::vector<position>& vertexes,
                                     size_t num_vertexes, color c)
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

triangle_indexed_render::triangle_indexed_render(
    std::array<color, buffer_size>& buffer, size_t width, size_t height)
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

triangle_interpolated::triangle_interpolated(
    std::array<color, buffer_size>& buffer, size_t width, size_t height)
    : triangle_indexed_render(buffer, width, height)
{
}

std::vector<vertex> triangle_interpolated::raster_horizontal_triangle(
    const vertex& single, const vertex& left, const vertex& right)
{
    std::vector<vertex> out;

    // 1. get first left and right points and draw horizontal line
    // 2. step to next left and right points and draw next horizontal line
    // 3. do the same till last single point

    size_t num_of_hlines = static_cast<size_t>(std::abs(single.f1 - left.f1));
    bool   direction     = single.f0 > left.f0; // up or down

    for (size_t i = 0; i <= num_of_hlines; ++i)
    {
        float  t_vertical   = static_cast<float>(i) / num_of_hlines;
        vertex left_vertex  = interpolate(left, single, t_vertical);
        vertex right_vertex = interpolate(right, single, t_vertical);

        size_t num_of_pixels_in_line =
            static_cast<size_t>(std::abs(left_vertex.f0 - right_vertex.f0));
        for (size_t p = 0; p <= num_of_pixels_in_line; ++p)
        {
            float  t_pixel = static_cast<float>(p) / num_of_pixels_in_line;
            vertex pixel   = interpolate(left_vertex, right_vertex, t_pixel);

            out.push_back(pixel);
        }
    }

    return out;
}

std::vector<vertex> triangle_interpolated::rasterize_triangle(const vertex& v0,
                                                              const vertex& v1,
                                                              const vertex& v2)
{
    std::vector<vertex> out;

    // common idea:
    // 1. sort input vertexes in order top to bottom
    // 2. build two horizontal triangles from first two vertexes and one middle
    // 3. interpolate two horizontal triangles with horizontal lines

    // sort by Y position input triangles:
    std::array<const vertex*, 3> in_vertexes{ &v0, &v1, &v2 };
    std::sort(begin(in_vertexes), end(in_vertexes),
              [](const vertex* left, const vertex* right) {
                  return left->f1 < left->f1;
              });

    const vertex& top    = *in_vertexes.at(0);
    const vertex& middle = *in_vertexes.at(1);
    const vertex& bottom = *in_vertexes.at(2);

    // first and last vertex will be longest triangle side
    // we need to find middle point on longest triangle side with same Y
    // coordinate like in middle vertex after sort
    position start{ static_cast<int32_t>(top.f0),
                    static_cast<int32_t>(top.f1) };
    position end{ static_cast<int32_t>(bottom.f0),
                  static_cast<int32_t>(bottom.f1) };

    std::vector<position> longest_side_line = pixels_positions(start, end);

    auto it_middle =
        std::find_if(begin(longest_side_line), end(longest_side_line),
                     [&](const position& pos) {
                         return pos.y == static_cast<int32_t>(middle.f1);
                     });
    assert(it_middle != end(longest_side_line));
    position second_middle = *it_middle;

    // interpolate second_middle position to get 4 vertex
    float  t = (second_middle - start).length() / (end - start).length();
    vertex second_middle_vertex = interpolate(top, bottom, t);

    // now render two horizontal triangles with horizontal lines
    // top triangle
    std::vector<vertex> top_triangle =
        raster_horizontal_triangle(top, middle, second_middle_vertex);
    std::vector<vertex> bottom_triangle =
        raster_horizontal_triangle(bottom, middle, second_middle_vertex);

    out.insert(end(out), begin(top_triangle), end(top_triangle));
    out.insert(end(out), begin(bottom_triangle), end(bottom_triangle));

    return out;
}

void triangle_interpolated::draw_triangles(std::vector<vertex>&  vertexes,
                                           std::vector<uint8_t>& indexes)
{
    for (size_t index = 0; index < indexes.size(); index += 3)
    {
        const uint8_t index0 = indexes.at(index + 0);
        const uint8_t index1 = indexes.at(index + 1);
        const uint8_t index2 = indexes.at(index + 2);

        const vertex& v0 = vertexes.at(index0);
        const vertex& v1 = vertexes.at(index1);
        const vertex& v2 = vertexes.at(index2);

        const vertex v0_ = program_->vertex_shader(v0);
        const vertex v1_ = program_->vertex_shader(v1);
        const vertex v2_ = program_->vertex_shader(v2);

        const std::vector<vertex> interpoleted{ rasterize_triangle(v0_, v1_,
                                                                   v2_) };
        for (const vertex& interpolated_vertex : interpoleted)
        {
            const color    c = program_->fragment_shader(interpolated_vertex);
            const position pos{ static_cast<int32_t>(interpolated_vertex.f0),
                                static_cast<int32_t>(interpolated_vertex.f1) };
            set_pixel(pos, c);
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

    save_image("01_lines.ppm", image);

    triangle_render render_tri(image, width, height);
    render_tri.clear(black);

    std::vector<position> triangle;
    triangle.push_back(position(0, 0));
    triangle.push_back(position(width - 1, height - 1));
    triangle.push_back(position(0, height - 1));

    render_tri.draw_triangles(triangle, 3, green);

    save_image("02_triangle.ppm", image);

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

            position v0(0 + i * step_x, 0 + j * step_y);
            position v1(v0.x + step_x, v0.y + step_y);
            position v2(v0.x, v0.y + step_y);
            position v3(v0.x + step_x, v0.y);

            triangles.push_back(v0);
            triangles.push_back(v1);
            triangles.push_back(v2);

            triangles.push_back(v0);
            triangles.push_back(v3);
            triangles.push_back(v1);
        }
    }

    render_tri.draw_triangles(triangles, triangles.size(), green);

    save_image("03_triangles.ppm", image);

    std::vector<position> triangles_for_index;

    int32_t step_x = (width - 1) / max_x;
    int32_t step_y = (height - 1) / max_y;

    for (size_t i = 0; i <= max_y; ++i)
    {
        for (size_t j = 0; j <= max_x; ++j)
        {
            position v(j * step_x, i * step_y);

            triangles_for_index.push_back(v);
        }
    }

    assert(triangles_for_index.size() == (max_x + 1) * (max_y + 1));

    std::vector<uint8_t> indexes;

    for (size_t x = 0; x < max_x; ++x)
    {
        for (size_t y = 0; y < max_y; ++y)
        {
            uint8_t index0 = y * (max_y + 1) + x;
            uint8_t index1 = index0 + (max_y + 1) + 1;
            uint8_t index2 = index1 - 1;
            uint8_t index3 = index0 + 1;

            indexes.push_back(index0);
            indexes.push_back(index1);
            indexes.push_back(index2);

            indexes.push_back(index0);
            indexes.push_back(index3);
            indexes.push_back(index1);
        }
    }

    triangle_indexed_render indexed_render(image, width, height);
    indexed_render.clear(black);

    indexed_render.draw_triangles(triangles_for_index, indexes, green);

    save_image("04_triangles_indexes.ppm", image);

    return 0;
}
