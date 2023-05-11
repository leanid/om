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

om::vertex blend_vertex(const om::vertex& vl,
                        const om::vertex& vr,
                        const float       a)
{
    om::vertex r;
    r.x = (1.0f - a) * vl.x + a * vr.x;
    r.y = (1.0f - a) * vl.y + a * vr.y;
    return r;
}

om::triangle blend(const om::triangle& tl,
                   const om::triangle& tr,
                   const float         a)
{
    om::triangle r;
    r.v[0] = blend_vertex(tl.v[0], tr.v[0], a);
    r.v[1] = blend_vertex(tl.v[1], tr.v[1], a);
    r.v[2] = blend_vertex(tl.v[2], tr.v[2], a);
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

    std::ifstream file("./01-basic-game-dev/05-1-vertex-morphing/vertexes.txt",
                       std::ios_base::binary);
    assert(!!file);

    om::triangle tr1q;
    om::triangle tr2q;

    om::triangle tr1t;
    om::triangle tr2t;

    file >> tr1q >> tr2q >> tr1t >> tr2t;

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

        float alpha = (std::sin(engine->get_time_from_init()) * 0.5f) + 0.5f;

        om::triangle tr1 = blend(tr1q, tr1t, alpha);
        om::triangle tr2 = blend(tr2q, tr2t, alpha);

        engine->render_triangle(tr1);
        engine->render_triangle(tr2);

        engine->swap_buffers();
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
