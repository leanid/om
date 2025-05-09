#include "04_triangle_interpolated_render.hxx"

#include <SDL3/SDL.h>

#include <cmath>
#include <cstdlib>

#include <algorithm>
#include <iostream>
// NOLINTNEXTLINE
int main(int, char**)
{
    using namespace std;

    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    constexpr size_t width  = 320;
    constexpr size_t height = 240;

    SDL_Window* window = SDL_CreateWindow(
        "runtime soft render", width, height, SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr)
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    const color black = { .r = 0, .g = 0, .b = 0 };

    canvas image(width, height);

    triangle_interpolated interpolated_render(image, width, height);

    struct program : gfx_program
    {
        double mouse_x{};
        double mouse_y{};
        double radius{};

        void set_uniforms(const uniforms& a_uniforms) override
        {
            mouse_x = a_uniforms.f0;
            mouse_y = a_uniforms.f1;
            radius  = a_uniforms.f2;
        }
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;

            double x = out.x;
            double y = out.y;

            double dx = x - mouse_x;
            double dy = y - mouse_y;
            if (dx * dx + dy * dy < radius * radius)
            {
                // un magnet from mouse
                double len = std::sqrt(dx * dx + dy * dy);
                if (len > 0)
                {
                    // normalize vector from vertex to mouse pos
                    double norm_dx = dx / len;
                    double norm_dy = dy / len;
                    // find position of point on radius from mouse pos in center
                    double radius_pos_x = mouse_x + norm_dx * radius;
                    double radius_pos_y = mouse_y + norm_dy * radius;
                    // find middle point
                    x = (x + radius_pos_x) / 2;
                    y = (y + radius_pos_y) / 2;
                }
            }

            assert(!std::isnan(x));
            assert(!std::isnan(y));
            out.x = x;
            out.y = y;

            return out;
        }
        color fragment_shader(const vertex& v_in) override
        {
            color out;
            out.r = static_cast<uint8_t>(v_in.f3 * 255);
            out.g = static_cast<uint8_t>(v_in.f4 * 255);
            out.b = static_cast<uint8_t>(v_in.f5 * 255);

            double x  = v_in.x;
            double y  = v_in.y;
            double dx = mouse_x - x;
            double dy = mouse_y - y;
            if (dx * dx + dy * dy < radius * radius)
            {
                double len          = std::sqrt(dx * dx + dy * dy);
                double green_to_red = (len / radius) - 0.35;
                green_to_red        = std::clamp(green_to_red, 0.0, 1.0);
                out.r = static_cast<uint8_t>((1 - green_to_red) * 255);
                out.g = static_cast<uint8_t>((green_to_red) * 255);
                out.b = 0;
            }

            return out;
        }
    } program01;

    std::vector<vertex> triangle_v;

    const size_t cell_x_count = 20;
    const size_t cell_y_count = 20;
    const double cell_width   = static_cast<double>(width) / cell_x_count;
    const double cell_height  = static_cast<double>(height) / cell_y_count;

    // generate vertexes for our cell mesh
    for (size_t j = 0; j < cell_y_count; ++j)
    {
        for (size_t i = 0; i < cell_x_count; ++i)
        {
            double x = i * cell_width;  // NOLINT
            double y = j * cell_height; // NOLINT
            double r = 0;
            double g = 1;
            double b = 0;
            double u = 0;
            double v = 0;
            triangle_v.push_back({ x, y, r, g, b, u, v, 0 });
        }
    }

    std::vector<uint16_t> indexes_v;

    // generate indexes for our cell mesh
    for (size_t j = 0; j < cell_y_count - 1; ++j)
    {
        for (size_t i = 0; i < cell_x_count - 1; ++i)
        {
            uint16_t v0 = j * cell_x_count + i;
            uint16_t v1 = v0 + 1;
            uint16_t v2 = v0 + cell_x_count;
            uint16_t v3 = v2 + 1;

            // add two triangles
            //  v0-----v1
            //  |     /|
            //  |    / |
            //  |   /  |
            //  |  /   |
            //  | /    |
            //  v2-----v3
            // we want only cells without internal color fill
            // so generate "triangle" for every age
            bool want_only_cell_border = true;
            if (want_only_cell_border)
            {
                indexes_v.insert(end(indexes_v), { v0, v1, v0 });
                // indexes_v.insert(end(indexes_v), { v1, v2, v1 });
                indexes_v.insert(end(indexes_v), { v2, v0, v2 });

                indexes_v.insert(end(indexes_v), { v2, v3, v2 });
                indexes_v.insert(end(indexes_v), { v1, v3, v1 });
            }
            else
            {
                indexes_v.insert(end(indexes_v), { v0, v1, v2 });
                indexes_v.insert(end(indexes_v), { v2, v1, v3 });
            }
        }
    }

    void*     pixels = image.get_pixels().data();
    const int depth  = sizeof(color) * 8;
    const int pitch  = width * sizeof(color);
    const int rmask  = 0x000000ff;
    const int gmask  = 0x0000ff00;
    const int bmask  = 0x00ff0000;
    const int amask  = 0;

    interpolated_render.set_gfx_program(program01);

    double mouse_x{ 1000 };
    double mouse_y{ 100 };
    double radius{ 40.0 }; // 20 pixels radius

    bool continue_loop = true;

    while (continue_loop)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                continue_loop = false;
                break;
            }
            else if (e.type == SDL_EVENT_MOUSE_MOTION)
            {
                mouse_x = e.motion.x;
                mouse_y = e.motion.y;
            }
            else if (e.type == SDL_EVENT_MOUSE_WHEEL)
            {
                radius += e.wheel.y;
            }
        }

        interpolated_render.clear(black);
        program01.set_uniforms(
            uniforms{ .f0 = mouse_x, .f1 = mouse_y, .f2 = radius });

        interpolated_render.draw_triangles(triangle_v, indexes_v);

        SDL_Surface* bitmapSurface = SDL_CreateSurfaceFrom(
            width, height, SDL_PIXELFORMAT_RGB24, pixels, pitch);
        if (bitmapSurface == nullptr)
        {
            cerr << SDL_GetError() << endl;
            return EXIT_FAILURE;
        }
        SDL_Texture* bitmapTex =
            SDL_CreateTextureFromSurface(renderer, bitmapSurface);
        if (bitmapTex == nullptr)
        {
            cerr << SDL_GetError() << endl;
            return EXIT_FAILURE;
        }
        SDL_DestroySurface(bitmapSurface);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, bitmapTex, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_DestroyTexture(bitmapTex);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}
