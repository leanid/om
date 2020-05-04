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

#include "loader_obj.hxx"

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

om::tri0 to_om_triangle(const loader_obj&       mesh_data,
                        const loader_obj::face& face)
{
    om::tri0 result;

    const float scale_x = 600.f / 800.f; // window_width / window_height
    const float scale_y = 1.f;

    const loader_obj::vertex& v0 = mesh_data.vertexes().at(face.p0.vtn.v - 1);
    result.v[0].p                = om::pos{ v0.x * scale_x, v0.y * scale_y };

    const loader_obj::vertex& v1 = mesh_data.vertexes().at(face.p1.vtn.v - 1);
    result.v[1].p                = om::pos{ v1.x * scale_x, v1.y * scale_y };

    const loader_obj::vertex& v2 = mesh_data.vertexes().at(face.p2.vtn.v - 1);
    result.v[2].p                = om::pos{ v2.x * scale_x, v2.y * scale_y };

    return result;
}

int main(int /*argc*/, char* /*argv*/[])
{
    std::ifstream file("circle.obj", std::ios::binary);

    loader_obj circle_data(file);

    std::ifstream file_next("hart.obj", std::ios::binary);
    loader_obj    hart_data(file_next);

    // for morphing animation
    assert(hart_data.vertexes().size() == circle_data.vertexes().size());
    assert(hart_data.faces().size() == circle_data.faces().size());

    std::unique_ptr<om::engine, void (*)(om::engine*)> engine(
        om::create_engine(), om::destroy_engine);

    const std::string error = engine->initialize("");
    if (!error.empty())
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }

    om::texture* texture = engine->create_texture("tank.png");
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
            for (auto& face : circle_data.faces())
            {
                om::tri0 tri0 = to_om_triangle(circle_data, face);
                om::tri0 tri1 = to_om_triangle(hart_data, face);

                float time  = engine->get_time_from_init();
                float alpha = 0.5f * std::sin(time) + 0.5f;

                om::tri0 t1 = blend(tri0, tri1, alpha);

                engine->render(t1, om::color(1.f, 0.f, 0.f, 1.f));
            }
        }

        if (current_shader == 1)
        {
            std::ifstream file("vert_pos_color.txt");
            assert(!!file);

            om::tri1 tr1;
            om::tri1 tr2;

            file >> tr1 >> tr2;

            engine->render(tr1);
            engine->render(tr2);
        }

        if (current_shader == 2)
        {
            std::ifstream file("vert_tex_color.txt");
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
