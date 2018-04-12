#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string_view>

#include "engine.hxx"

int main(int /*argc*/, char* /*argv*/ [])
{
    std::unique_ptr<om::engine, void (*)(om::engine*)> engine(
        om::create_engine(), om::destroy_engine);

    std::string err = engine->initialize("");
    if (!err.empty())
    {
    	std::cerr << err << std::endl;
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
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
