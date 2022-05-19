#include "04_triangle_interpolated_render.hxx"

#include <algorithm>
#include <iostream>

double interpolate(const double f0, const double f1, const double t)
{
    assert(t >= 0.0);
    assert(t <= 1.0);
    return f0 + (f1 - f0) * t;
}

vertex interpolate(const vertex& v0, const vertex& v1, const double t)
{
    return { interpolate(v0.x, v1.x, t),   interpolate(v0.y, v1.y, t),
             interpolate(v0.z, v1.z, t),   interpolate(v0.f3, v1.f3, t),
             interpolate(v0.f4, v1.f4, t), interpolate(v0.f5, v1.f5, t),
             interpolate(v0.f6, v1.f6, t), interpolate(v0.f7, v1.f7, t) };
}

triangle_interpolated::triangle_interpolated(canvas& buffer,
                                             size_t  width,
                                             size_t  height)
    : triangle_indexed_render(buffer, width, height)
{
}

void triangle_interpolated::raster_one_horizontal_line(
    const vertex&        left_vertex,
    const vertex&        right_vertex,
    std::vector<vertex>& out)
{
    size_t num_of_pixels_in_line = static_cast<size_t>(
        std::round(std::abs(left_vertex.x - right_vertex.x)));
    if (num_of_pixels_in_line > 0)
    {
        // use +1 pixels to garantee no empty black pixels
        for (size_t p = 0; p <= num_of_pixels_in_line + 1; ++p)
        {
            double t_pixel =
                static_cast<double>(p) / (num_of_pixels_in_line + 1);
            vertex pixel = interpolate(left_vertex, right_vertex, t_pixel);

            out.push_back(pixel);
        }
    }
    else
    {
        out.push_back(left_vertex);
    }
}

std::vector<vertex> triangle_interpolated::raster_horizontal_triangle(
    const vertex& single, const vertex& left, const vertex& right)
{
    std::vector<vertex> out;

    // 1. get first left and right points and draw horizontal line
    // 2. step to next left and right points and draw next horizontal line
    // 3. do the same till last single point

    size_t num_of_hlines =
        static_cast<size_t>(std::round(std::abs(single.y - left.y)));

    if (num_of_hlines > 0)
    {
        for (size_t i = 0; i <= num_of_hlines; ++i)
        {
            double t_vertical   = static_cast<double>(i) / num_of_hlines;
            vertex left_vertex  = interpolate(left, single, t_vertical);
            vertex right_vertex = interpolate(right, single, t_vertical);

            raster_one_horizontal_line(left_vertex, right_vertex, out);
        }
    }
    else
    {
        raster_one_horizontal_line(left, right, out);
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
    std::sort(begin(in_vertexes),
              end(in_vertexes),
              [](const vertex* left, const vertex* right)
              { return left->y < right->y; });

    const vertex& top    = *in_vertexes.at(0);
    const vertex& middle = *in_vertexes.at(1);
    const vertex& bottom = *in_vertexes.at(2);

    // first and last vertex will be longest triangle side
    // we need to find middle point on longest triangle side with same Y
    // coordinate like in middle vertex after sort
    position start{ static_cast<int32_t>(std::round(top.x)),
                    static_cast<int32_t>(std::round(top.y)) };
    position end{ static_cast<int32_t>(std::round(bottom.x)),
                  static_cast<int32_t>(std::round(bottom.y)) };
    position middle_pos{ static_cast<int32_t>(std::round(middle.x)),
                         static_cast<int32_t>(std::round(middle.y)) };

    // Here 3 quik and durty HACK if triangle consist from same points
    if (start == end)
    {
        // just render line start -> middle

        position delta        = start - middle_pos;
        size_t   count_pixels = 4 * (std::abs(delta.x) + std::abs(delta.y) + 1);
        for (size_t i = 0; i < count_pixels; ++i)
        {
            double t      = static_cast<double>(i) / count_pixels;
            vertex vertex = interpolate(top, middle, t);
            out.push_back(vertex);
        }

        return out;
    }

    if (start == middle_pos)
    {
        // just render line start -> middle

        position delta        = start - end;
        size_t   count_pixels = 4 * (std::abs(delta.x) + std::abs(delta.y) + 1);
        for (size_t i = 0; i < count_pixels; ++i)
        {
            double t      = static_cast<double>(i) / count_pixels;
            vertex vertex = interpolate(top, bottom, t);
            out.push_back(vertex);
        }

        return out;
    }

    if (end == middle_pos)
    {
        // just render line start -> middle

        position delta        = start - middle_pos;
        size_t   count_pixels = 4 * (std::abs(delta.x) + std::abs(delta.y) + 1);
        for (size_t i = 0; i < count_pixels; ++i)
        {
            double t      = static_cast<double>(i) / count_pixels;
            vertex vertex = interpolate(top, middle, t);
            out.push_back(vertex);
        }

        return out;
    }

    std::vector<position> longest_side_line = pixels_positions(start, end);

    auto it_middle = std::find_if(
        begin(longest_side_line),
        std::end(longest_side_line),
        [&](const position& pos)
        { return pos.y == static_cast<int32_t>(std::round(middle.y)); });
    assert(it_middle != std::end(longest_side_line));
    position second_middle = *it_middle;

    // interpolate second_middle position to get 4 vertex
    double t{ 0 };
    double end_start = (end - start).length();
    if (end_start > 0)
    {
        double middle_start = (second_middle - start).length();
        t                   = middle_start / end_start;
    }
    else
    {
        // start == end so we need just render line
        std::vector<position> line = pixels_positions(start, middle_pos);
    }
    vertex second_middle_vertex = interpolate(top, bottom, t);

    // now render two horizontal triangles with horizontal lines
    // top triangle
    std::vector<vertex> top_triangle =
        raster_horizontal_triangle(top, middle, second_middle_vertex);

    std::vector<vertex> bottom_triangle =
        raster_horizontal_triangle(bottom, middle, second_middle_vertex);

    out.insert(std::end(out), begin(top_triangle), std::end(top_triangle));
    out.insert(
        std::end(out), begin(bottom_triangle), std::end(bottom_triangle));

    return out;
}

void triangle_interpolated::draw_triangles(std::vector<vertex>&   vertexes,
                                           std::vector<uint16_t>& indexes)
{
    for (size_t index = 0; index < indexes.size(); index += 3)
    {
        const uint16_t index0 = indexes.at(index + 0);
        const uint16_t index1 = indexes.at(index + 1);
        const uint16_t index2 = indexes.at(index + 2);

        const vertex& v0 = vertexes.at(index0);
        const vertex& v1 = vertexes.at(index1);
        const vertex& v2 = vertexes.at(index2);

        const vertex v0_ = program_->vertex_shader(v0);
        const vertex v1_ = program_->vertex_shader(v1);
        const vertex v2_ = program_->vertex_shader(v2);

        const std::vector<vertex> interpoleted{ rasterize_triangle(
            v0_, v1_, v2_) };
        for (const vertex& interpolated_vertex : interpoleted)
        {
            const color    c = program_->fragment_shader(interpolated_vertex);
            const position pos{
                static_cast<int32_t>(std::round(interpolated_vertex.x)),
                static_cast<int32_t>(std::round(interpolated_vertex.y))
            };
            set_pixel(pos, c);
        }
    }
}
