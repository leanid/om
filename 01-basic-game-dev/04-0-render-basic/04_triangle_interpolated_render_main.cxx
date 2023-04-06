#include "04_triangle_interpolated_render.hxx"

int main(int, char**)
{
    const color black = { 0, 0, 0 };

    constexpr size_t width  = 320;
    constexpr size_t height = 240;

    canvas image(width, height);

    triangle_interpolated interpolated_render(image, width, height);

    struct program : gfx_program
    {
        void   set_uniforms(const uniforms&) override {}
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;

            // rotate
            double alpha = 3.14159 / 6; // 30 degree
            double x     = out.x;
            double y     = out.y;
            out.x        = x * std::cos(alpha) - y * std::sin(alpha);
            out.y        = x * std::sin(alpha) + y * std::cos(alpha);

            // scale into 3 times
            out.x *= 0.3;
            out.y *= 0.3;

            // move
            out.x += (width / 2);
            out.y += (height / 2);

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

    interpolated_render.clear(black);
    interpolated_render.set_gfx_program(program01);

    std::vector<vertex>   triangle_v{ { 0, 0, 1, 0, 0, 0, 0, 0 },
                                      { 0, 239, 0, 1, 0, 0, 239, 0 },
                                      { 319, 239, 0, 0, 1, 319, 239, 0 } };
    std::vector<uint16_t> indexes_v{ 0, 1, 2 };

    interpolated_render.draw_triangles(triangle_v, indexes_v);

    image.save_image("05_interpolated.ppm");

    // texture example
    struct program_tex : gfx_program
    {
        std::vector<color> texture;

        void   set_uniforms(const uniforms&) override {}
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;
            /*
                        // rotate
                        double alpha = 3.14159 / 6; // 30 degree
                        double x     = out.f0;
                        double y     = out.f1;
                        out.f0       = x * std::cos(alpha) - y *
               std::sin(alpha); out.f1       = x * std::sin(alpha) + y *
               std::cos(alpha);
            */
            /*
            // scale into 3 times
            out.f0 *= 0.8;
            out.f1 *= 0.8;
            */
            /*
                        // move
                        out.f0 += (width / 2);
                        out.f1 += (height / 2);
            */
            return out;
        }
        color fragment_shader(const vertex& v_in) override
        {
            color out;

            out.r = static_cast<uint8_t>(v_in.f3 * 255);
            out.g = static_cast<uint8_t>(v_in.f4 * 255);
            out.b = static_cast<uint8_t>(v_in.f5 * 255);

            color from_texture = sample2d(v_in.f5, v_in.f6);
            out.r += from_texture.r;
            out.g = from_texture.g;
            out.b += from_texture.b;
            return out;
        }

        void set_texture(const std::vector<color>& tex) { texture = tex; }

        color sample2d(double u_, double v_)
        {
            uint32_t u = static_cast<uint32_t>(std::round(u_));
            uint32_t v = static_cast<uint32_t>(std::round(v_));

            color c = texture.at(v * width + u);
            return c;
        }
    } program02;

    program02.set_texture(image.get_pixels());

    interpolated_render.set_gfx_program(program02);

    interpolated_render.clear(black);

    interpolated_render.draw_triangles(triangle_v, indexes_v);

    image.save_image("06_textured_triangle.ppm");

    return 0;
}
