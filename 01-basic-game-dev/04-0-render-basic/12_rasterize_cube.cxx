#include "04_triangle_interpolated_render.hxx"

/// From 3D to 2D canvas
position project_vertex(const vertex& in);
position viewport_to_canvas(const vertex& in);
vertex   v3d(double x, double y, double z);
// NOLINTNEXTLINE
int main(int, char**)
{
    const color black = { .r = 0, .g = 0, .b = 0 };

    constexpr size_t width  = 320;
    constexpr size_t height = 240;

    canvas image(width, height);

    triangle_interpolated render(image, width, height);

    struct program : gfx_program
    {
        void   set_uniforms(const uniforms&) override {}
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;
            return out;
        }
        color fragment_shader(const vertex& v_in) override
        {
            color out;
            out.r = static_cast<uint8_t>(v_in.f3 * 255);
            out.g = static_cast<uint8_t>(v_in.f4 * 255);
            out.b = static_cast<uint8_t>(v_in.f5 * 255);
            return out;
        }
    } program01;

    render.clear(black);
    render.set_gfx_program(program01);

    // four "front" vertices
    vertex vAf = v3d(-1, 1, 1);
    vertex vBf = v3d(1, 1, 1);
    vertex vCf = v3d(1, -1, 1);
    vertex vDf = v3d(-1, -1, 1);

    // for "back" vertices
    vertex vAb = v3d(-1, 1, 2);
    vertex vBb = v3d(1, 1, 2);
    vertex vCb = v3d(1, -1, 2);
    vertex vDb = v3d(-1, -1, 2);

    // front face
    render.draw_line(project_vertex(vAf), project_vertex(vBf), color_blue);
    render.draw_line(project_vertex(vBf), project_vertex(vCf), color_blue);
    render.draw_line(project_vertex(vCf), project_vertex(vDf), color_blue);
    render.draw_line(project_vertex(vDf), project_vertex(vAf), color_blue);

    // back face
    render.draw_line(project_vertex(vAb), project_vertex(vBb), color_red);
    render.draw_line(project_vertex(vBb), project_vertex(vCb), color_red);
    render.draw_line(project_vertex(vCb), project_vertex(vDb), color_red);
    render.draw_line(project_vertex(vDb), project_vertex(vAb), color_red);

    // front-to-back edges
    render.draw_line(project_vertex(vAf), project_vertex(vAb), color_green);
    render.draw_line(project_vertex(vBf), project_vertex(vBb), color_green);
    render.draw_line(project_vertex(vCf), project_vertex(vCb), color_green);
    render.draw_line(project_vertex(vDf), project_vertex(vDb), color_green);

    image.save_image("12_rasterize_cube.ppm");

    return 0;
}

position project_vertex(const vertex& in)
{
    const double d = 1.0; // viewport distance from O in 3D space

    vertex tmp_v = in;
    tmp_v.x      = in.x * d / in.z;
    tmp_v.y      = in.y * d / in.z;
    tmp_v.z      = d;

    return viewport_to_canvas(tmp_v);
}

position viewport_to_canvas(const vertex& in)
{
    const double canvas_width  = 320 / 2.0;
    const double canvas_height = 240 / 2.0;

    const double viewport_width  = 1.0;
    const double viewport_height = 1.0;

    position out;

    out.x = in.x * (canvas_width - 1) / viewport_width + canvas_width; // NOLINT
    out.y =
        in.y * (canvas_height - 1) / viewport_height + canvas_height; // NOLINT

    return out;
}
// NOLINTNEXTLINE
vertex v3d(double x, double y, double z)
{
    vertex v;
    v.x  = x;
    v.y  = y;
    v.z  = z;
    v.f3 = 0;
    v.f4 = 0;
    v.f5 = 0;
    v.f6 = 0;
    v.f7 = 0;
    return v;
}
