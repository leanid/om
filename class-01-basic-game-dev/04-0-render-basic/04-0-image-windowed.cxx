#include "04_triangle_interpolated_render.hxx"

#include <SDL2/SDL.h>

#include <cmath>
#include <cstdlib>

#include <algorithm>
#include <iostream>

int main(int, char**)
{
    using namespace std;

    if (0 != SDL_Init(SDL_INIT_EVERYTHING))
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    constexpr size_t width  = 320;
    constexpr size_t height = 240;

    SDL_Window* window = SDL_CreateWindow("runtime soft render",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          width,
                                          height,
                                          SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    const color black = { 0, 0, 0 };

    canvas texture(0, 0);
    texture.load_image("leo.ppm");

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

            float tex_x = v_in.f5; // 0..1
            float tex_y = v_in.f6; // 0..1

            canvas* texture = uniforms_.texture0;

            size_t tex_width  = texture->get_width();
            size_t tex_height = texture->get_height();

            size_t t_x = static_cast<size_t>((tex_width - 1) * tex_x);
            size_t t_y = static_cast<size_t>((tex_height - 1) * tex_y);

            out = texture->get_pixel(t_x, t_y);

            return out;
        }
    } program01;

    struct grayscale : program
    {
        color fragment_shader(const vertex& v_in) override
        {
            color out;

            float tex_x = v_in.f5; // 0..1
            float tex_y = v_in.f6; // 0..1

            canvas* texture = uniforms_.texture0;

            size_t tex_width  = texture->get_width();
            size_t tex_height = texture->get_height();

            size_t t_x = static_cast<size_t>((tex_width - 1) * tex_x);
            size_t t_y = static_cast<size_t>((tex_height - 1) * tex_y);

            out = texture->get_pixel(t_x, t_y);

            uint8_t gray = static_cast<uint8_t>(
                0.2125 * out.r + 0.7152 * out.g + 0.0721 * out.b);

            out.r = gray;
            out.g = gray;
            out.b = gray;

            return out;
        }
    } program02;

    std::array<gfx_program*, 2> programs{ &program01, &program02 };
    size_t                      current_program_index = 0;
    gfx_program* current_program = programs.at(current_program_index);

    size_t w = width;
    size_t h = height;

    // clang-format off
    //                                x  y          r  g  b  tx ty
    std::vector<vertex> triangle_v{ { 0, 0,         1, 1, 1, 0, 0, 0 },
                                    { w - 1, h - 1, 1, 1, 1, 1, 1, 0 },
                                    { 0, h - 1,     1, 1, 1, 0, 1, 0 },
                                    { w - 1, 0,     1, 1, 1, 1, 0, 0 } };
    // clang-format on
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
            if (e.type == SDL_QUIT)
            {
                continue_loop = false;
                break;
            }
            else if (e.type == SDL_KEYUP)
            {
                current_program_index =
                    (current_program_index + 1) % programs.size();
                current_program = programs.at(current_program_index);

                interpolated_render.set_gfx_program(*current_program);
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
            }
            else if (e.type == SDL_MOUSEWHEEL)
            {
            }
        }

        interpolated_render.clear(black);
        current_program->set_uniforms(
            uniforms{ 0, 0, 0, 0, 0, 0, 0, 0, &texture });

        interpolated_render.draw_triangles(triangle_v, indexes_v);

        SDL_Surface* bitmapSurface = SDL_CreateRGBSurfaceFrom(
            pixels, width, height, depth, pitch, rmask, gmask, bmask, amask);
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
        SDL_FreeSurface(bitmapSurface);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bitmapTex, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_DestroyTexture(bitmapTex);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}
