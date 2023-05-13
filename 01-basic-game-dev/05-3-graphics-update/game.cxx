#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

#include "engine.hxx"

om::v0 blend(const om::v0& vl, const om::v0& vr, const float a)
{
    om::v0 r;
    r.p.x = (1.0f - a) * vl.p.x + a * vr.p.x;
    r.p.y = (1.0f - a) * vl.p.y + a * vr.p.y;
    return r;
}

om::tri0 blend(const om::tri0& tl, const om::tri0& tr, const float a)
{
    om::tri0 r;
    r.v[0] = blend(tl.v[0], tr.v[0], a);
    r.v[1] = blend(tl.v[1], tr.v[1], a);
    r.v[2] = blend(tl.v[2], tr.v[2], a);
    return r;
}

int main(int /*argc*/, char* /*argv*/[])
{
    std::unique_ptr<om::engine, void (*)(om::engine*)> engine(
        om::create_engine(), om::destroy_engine);

    const std::string error = engine->initialize("");
    if (!error.empty())
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }

    om::texture* texture = engine->create_texture(
        "./01-basic-game-dev/05-3-graphics-update/tank.png");
    if (nullptr == texture)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }

    bool continue_loop  = true;
    int  current_shader = 0;
    while (continue_loop)
    {
        om::event event;

        while (engine->read_input(event))
        {
            std::cout << event << std::endl;
            switch (event)
            {
                case om::event::turn_off:
                    continue_loop = false;
                    break;
                case om::event::button1_released:
                    ++current_shader;
                    if (current_shader > 2)
                    {
                        current_shader = 0;
                    }
                    break;
                default:

                    break;
            }
        }

        if (current_shader == 0)
        {
            std::ifstream file(
                "./01-basic-game-dev/05-3-graphics-update/vert_pos.txt");
            assert(!!file);

            om::tri0 tr1;
            om::tri0 tr2;
            om::tri0 tr11;
            om::tri0 tr22;

            file >> tr1 >> tr2 >> tr11 >> tr22;

            float time  = engine->get_time_from_init();
            float alpha = std::sin(time);

            om::tri0 t1 = blend(tr1, tr11, alpha);
            om::tri0 t2 = blend(tr2, tr22, alpha);

            engine->render(t1, om::color(1.f, 0.f, 0.f, 1.f));
            engine->render(t2, om::color(0.f, 1.f, 0.f, 1.f));
        }

        if (current_shader == 1)
        {
            std::ifstream file(
                "./01-basic-game-dev/05-3-graphics-update/vert_pos_color.txt");
            assert(!!file);

            om::tri1 tr1;
            om::tri1 tr2;

            file >> tr1 >> tr2;

            engine->render(tr1);
            engine->render(tr2);
        }

        if (current_shader == 2)
        {
            std::ifstream file(
                "./01-basic-game-dev/05-3-graphics-update/vert_tex_color.txt");
            assert(!!file);

            om::tri2 tr1;
            om::tri2 tr2;

            file >> tr1 >> tr2;

            float time = engine->get_time_from_init();
            float s    = std::sin(time);
            float c    = std::cos(time);

            // animate one triangle texture coordinates
            for (auto& v : tr1.v)
            {
                v.uv.u += c;
                v.uv.v += s;
            }

            engine->render(tr1, texture);
            engine->render(tr2, texture);
        }

        engine->swap_buffers();
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
