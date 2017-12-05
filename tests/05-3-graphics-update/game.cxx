#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

#include "engine.hxx"

om::v0 blend_vertex(const om::v0& vl, const om::v0& vr, const float a)
{
    om::v0 r;
    r.p.x = (1.0f - a) * vl.p.x + a * vr.p.x;
    r.p.y = (1.0f - a) * vl.p.y + a * vr.p.y;
    return r;
}

om::tri0 blend(const om::tri0& tl, const om::tri0& tr, const float a)
{
    om::tri0 r;
    r.v[0] = blend_vertex(tl.v[0], tr.v[0], a);
    r.v[1] = blend_vertex(tl.v[1], tr.v[1], a);
    r.v[2] = blend_vertex(tl.v[2], tr.v[2], a);
    return r;
}

int main(int /*argc*/, char* /*argv*/ [])
{
    std::unique_ptr<om::engine, void (*)(om::engine*)> engine(
        om::create_engine(), om::destroy_engine);

    const std::string error = engine->initialize("");
    if (!error.empty())
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }

    bool continue_loop = true;
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
                default:
                    break;
            }
        }

        std::ifstream file("vert_pos.txt");
        assert(!!file);

        om::tri0 tr1;
        om::tri0 tr2;

        file >> tr1 >> tr2;

        engine->render(tr1, om::color(1.f, 0.f, 0.f, 1.f));
        engine->render(tr2, om::color(0.f, 0.f, 1.f, 1.f));

        engine->swap_buffers();
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
