#include "04_triangle_interpolated_render.hxx"

#include <SDL3/SDL.h>

#include <bitset>
#include <cmath>
#include <cstdlib>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <numbers>

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

    SDL_Renderer* renderer = SDL_CreateRenderer(window, "opengl");
    if (renderer == nullptr)
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    const color black = { .r = 0, .g = 0, .b = 0 };

    canvas texture(0, 0);
    texture.load_image("./01-basic-game-dev/04-0-render-basic/leo.ppm");

    canvas image(width, height);

    triangle_interpolated interpolated_render(image, width, height);

    struct program : gfx_program
    {
        uniforms uniforms_;
        void     set_uniforms(const uniforms& a_uniforms) override
        {
            uniforms_ = a_uniforms;
        }
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;
            return out;
        }
        color fragment_shader(const vertex& v_in) override
        {
            color out;

            float tex_x = v_in.f5; // 0..1 // NOLINT
            float tex_y = v_in.f6; // 0..1 // NOLINT

            canvas* texture = uniforms_.texture0;

            size_t tex_width  = texture->get_width();
            size_t tex_height = texture->get_height();

            auto t_x = static_cast<size_t>((tex_width - 1) * tex_x);  // NOLINT
            auto t_y = static_cast<size_t>((tex_height - 1) * tex_y); // NOLINT

            out = texture->get_pixel(t_x, t_y);

            return out;
        }
    } program01;

    struct grayscale : program
    {
        color fragment_shader(const vertex& v_in) override
        {
            color out;

            float tex_x = v_in.f5; // 0..1 // NOLINT
            float tex_y = v_in.f6; // 0..1 // NOLINT

            canvas* texture = uniforms_.texture0;

            size_t tex_width  = texture->get_width();
            size_t tex_height = texture->get_height();

            auto t_x = static_cast<size_t>((tex_width - 1) * tex_x);  // NOLINT
            auto t_y = static_cast<size_t>((tex_height - 1) * tex_y); // NOLINT

            out = texture->get_pixel(t_x, t_y);

            auto gray = static_cast<uint8_t>(0.2125 * out.r + 0.7152 * out.g +
                                             0.0721 * out.b);

            out.r = gray;
            out.g = gray;
            out.b = gray;

            return out;
        }
    } program02;

    struct rotate_image : program
    {
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;
            out.x -= (320 / 2);
            out.y -= (240 / 2);

            out.x *= 0.5;
            out.y *= 0.5;

            // rotate
            double alpha = (std::numbers::pi / 2) * uniforms_.f7 * -1;
            double x     = out.x;
            double y     = out.y;
            out.x        = x * std::cos(alpha) - y * std::sin(alpha);
            out.y        = x * std::sin(alpha) + y * std::cos(alpha);

            out.x += (320 / 2);
            out.y += (240 / 2);

            return out;
        }
    } program03;

    std::array<gfx_program*, 3> programs{ &program01, &program02, &program03 };
    size_t                      current_program_index = 0;
    gfx_program* current_program = programs.at(current_program_index);

    size_t w = width;
    size_t h = height;
    // NOLINTBEGIN(*)
    // clang-format off
    //                                x  y              r  g  b  tx ty
    std::vector<vertex> triangle_v{ { .x=0, .y=0,             .z=1, .f3=1, .f4=1, .f5=0, .f6=0, .f7=0 },
                                    { .x=w - 1.0, .y=h - 1.0, .z=1, .f3=1, .f4=1, .f5=1, .f6=1, .f7=0 },
                                    { .x=0, .y=h - 1.0,       .z=1, .f3=1, .f4=1, .f5=0, .f6=1, .f7=0 },
                                    { .x=w - 1.0, .y=0,       .z=1, .f3=1, .f4=1, .f5=1, .f6=0, .f7=0 } };
    // clang-format on
    // NOLINTEND(*)
    std::vector<uint16_t> indexes_v{ { 0, 1, 2, 0, 3, 1 } };

    void*     pixels = image.get_pixels().data();
    const int depth  = sizeof(color) * 8;
    const int pitch  = width * sizeof(color);
    const int rmask  = 0x000000ff;
    const int gmask  = 0x0000ff00;
    const int bmask  = 0x00ff0000;
    const int amask  = 0;

    interpolated_render.set_gfx_program(*current_program);

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
            else if (e.type == SDL_EVENT_KEY_UP)
            {
                current_program_index =
                    (current_program_index + 1) % programs.size();
                current_program = programs.at(current_program_index);

                interpolated_render.set_gfx_program(*current_program);
            }
            else if (e.type == SDL_EVENT_MOUSE_MOTION)
            {
            }
            else if (e.type == SDL_EVENT_MOUSE_WHEEL)
            {
            }
        }

        interpolated_render.clear(black);
        double time_from_start = SDL_GetTicks() / 1000.0;
        current_program->set_uniforms(uniforms{ .f0       = 0,
                                                .f1       = 0,
                                                .f2       = 0,
                                                .f3       = 0,
                                                .f4       = 0,
                                                .f5       = 0,
                                                .f6       = 0,
                                                .f7       = time_from_start,
                                                .texture0 = &texture });

        interpolated_render.draw_triangles(triangle_v, indexes_v);

        SDL_Surface* bitmapSurface = SDL_CreateSurfaceFrom(
            width, height, SDL_PIXELFORMAT_RGB24, pixels, pitch);
        if (bitmapSurface == nullptr)
        {
            cerr << "Failed to SDL_CreateSurfaceFrom(" << pixels << ", "
                 << width << ", " << height << ", " << pitch << ", "
                 << SDL_GetPixelFormatName(SDL_PIXELFORMAT_XRGB8888) << ") "
                 << SDL_GetError() << endl;
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
