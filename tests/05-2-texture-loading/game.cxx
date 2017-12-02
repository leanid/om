#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

#include "engine.hxx"

om::vertex blend_vertex(const om::vertex& vl, const om::vertex& vr,
                        const float a)
{
    om::vertex r;
    r.x = (1.0f - a) * vl.x + a * vr.x;
    r.y = (1.0f - a) * vl.y + a * vr.y;
    return r;
}

om::triangle blend(const om::triangle& tl, const om::triangle& tr,
                   const float a)
{
    om::triangle r;
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

        std::ifstream file("vert_and_tex_coord.txt");
        assert(!!file);

        om::triangle tr1;
        om::triangle tr2;

        file >> tr1 >> tr2;

        engine->render_triangle(tr1);
        engine->render_triangle(tr2);

        engine->swap_buffers();
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
