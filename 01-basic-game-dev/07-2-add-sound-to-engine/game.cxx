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
        "01-basic-game-dev/07-2-add-sound-to-engine/tank.png");
    if (nullptr == texture)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }

    om::vertex_buffer* vertex_buf = nullptr;

    std::ifstream file(
        "01-basic-game-dev/07-2-add-sound-to-engine/vert_tex_color.txt");
    if (!file)
    {
        std::cerr << "can't load vert_tex_color.txt\n";
        return EXIT_FAILURE;
    }
    else
    {
        std::array<om::tri2, 2> tr;
        file >> tr[0] >> tr[1];
        vertex_buf = engine->create_vertex_buffer(&tr[0], tr.size());
        if (vertex_buf == nullptr)
        {
            std::cerr << "can't create vertex buffer\n";
            return EXIT_FAILURE;
        }
    }

    om::sound_buffer* s = engine->create_sound_buffer(
        "01-basic-game-dev/07-2-add-sound-to-engine/t2_no_problemo.wav");
    om::sound_buffer* music = engine->create_sound_buffer(
        "01-basic-game-dev/07-2-add-sound-to-engine/8-bit_detective.wav");
    assert(music != nullptr);

    music->play(om::sound_buffer::properties::looped);

    bool continue_loop = true;

    om::vec2    current_tank_pos(0.f, 0.f);
    float       current_tank_direction(0.f);
    const float pi = 3.1415926f;

    while (continue_loop)
    {
        om::event event;

        while (engine->read_event(event))
        {
            std::cout << event << std::endl;
            switch (event.type)
            {
                case om::event_type::hardware:
                    if (std::get<om::hardware_data>(event.info).is_reset)
                    {
                        continue_loop = false;
                    }
                    break;
                case om::event_type::input_key:
                {
                    const auto& key_data = std::get<om::input_data>(event.info);
                    if (key_data.is_down)
                    {
                        if (key_data.key == om::keys::button1)
                        {
                            s->play(om::sound_buffer::properties::once);
                        }
                        else if (key_data.key == om::keys::button2)
                        {
                            s->play(om::sound_buffer::properties::looped);
                        }
                    }
                }
                break;
            }
        }

        if (engine->is_key_down(om::keys::left))
        {
            current_tank_pos.x -= 0.01f;
            current_tank_direction = -pi / 2.f;
        }
        else if (engine->is_key_down(om::keys::right))
        {
            current_tank_pos.x += 0.01f;
            current_tank_direction = pi / 2.f;
        }
        else if (engine->is_key_down(om::keys::up))
        {
            current_tank_pos.y += 0.01f;
            current_tank_direction = 0.f;
        }
        else if (engine->is_key_down(om::keys::down))
        {
            current_tank_pos.y -= 0.01f;
            current_tank_direction = -pi;
        }

        om::mat2x3 move   = om::mat2x3::move(current_tank_pos);
        om::mat2x3 aspect = om::mat2x3::scale(1, 640.f / 480.f);
        om::mat2x3 rot    = om::mat2x3::rotation(current_tank_direction);
        om::mat2x3 m      = rot * move * aspect;

        engine->render(*vertex_buf, texture, m);

        engine->swap_buffers();
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
