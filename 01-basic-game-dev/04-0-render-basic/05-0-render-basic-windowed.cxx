#include "04_triangle_interpolated_render.hxx"

#include <SDL3/SDL.h>

#include <cstdlib>
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

    size_t width  = 320;
    size_t height = 240;

    SDL_Window* window = SDL_CreateWindow(
        "runtime soft render", width, height, SDL_WINDOW_OPENGL); // NOLINT
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
                // make pixel gray if mouse cursor around current pixel with
                // radius
                // gray scale with formula: 0.21 R + 0.72 G + 0.07 B.
                double gray = 0.21 * out.r + 0.72 * out.g + 0.07 * out.b;
                out.r       = static_cast<uint8_t>(gray);
                out.g       = static_cast<uint8_t>(gray);
                out.b       = static_cast<uint8_t>(gray);
            }

            return out;
        }
    } program01;

    std::vector<vertex> triangle_v{
        { .x = 0, .y = 0, .z = 1, .f3 = 0, .f4 = 0, .f5 = 0, .f6 = 0, .f7 = 0 },
        { .x  = 0,
          .y  = 239,
          .z  = 0,
          .f3 = 1,
          .f4 = 0,
          .f5 = 0,
          .f6 = 239,
          .f7 = 0 },
        { .x  = 319,
          .y  = 239,
          .z  = 0,
          .f3 = 0,
          .f4 = 1,
          .f5 = 319,
          .f6 = 239,
          .f7 = 0 }
    };
    std::vector<uint16_t> indexes_v{ 0, 1, 2 };

    void*     pixels = image.get_pixels().data();
    const int depth  = sizeof(color) * 8;
    const int pitch  = static_cast<int>(width * sizeof(color));
    const int rmask  = 0x000000ff;
    const int gmask  = 0x0000ff00;
    const int bmask  = 0x00ff0000;
    const int amask  = 0;

    interpolated_render.set_gfx_program(program01);

    double mouse_x{};
    double mouse_y{};
    double radius{ 20.0 }; // 20 pixels radius

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
            width, height, SDL_PIXELFORMAT_RGB24, pixels, pitch); // NOLINT
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
