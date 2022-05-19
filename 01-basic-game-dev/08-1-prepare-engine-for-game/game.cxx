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

class tanks_game final : public om::lila
{
public:
    explicit tanks_game(om::engine& engine)
        : engine(engine)
    {
    }

    void on_initialize() final;
    void on_event(om::event&) final;
    void on_update(std::chrono::milliseconds frame_delta) final;
    void on_render() const final;

private:
    om::engine&  engine;
    om::texture* texture    = nullptr;
    om::vbo*     vertex_buf = nullptr;
    om::sound*   snd        = nullptr;

    om::vec2               current_tank_pos       = om::vec2(0.f, 0.f);
    float                  current_tank_direction = 0.f;
    static constexpr float pi                     = 3.1415926f;
};

std::unique_ptr<om::lila> om_tat_sat(om::engine& e)
{
    e.log << "creating main game object..." << std::endl;
    auto game = std::make_unique<tanks_game>(e);
    e.log << "finish creating main game object" << std::endl;
    return game;
}

void tanks_game::on_initialize()
{
    texture = engine.create_texture("tank.png");
    if (nullptr == texture)
    {
        engine.log << "failed load texture\n";
        return;
    }

    vertex_buf = nullptr;

    std::ifstream file("vert_tex_color.txt");
    if (!file)
    {
        engine.log << "can't load vert_tex_color.txt\n";
        return;
    }
    else
    {
        std::array<om::tri2, 2> tr;
        file >> tr[0] >> tr[1];
        vertex_buf = engine.create_vbo(&tr[0], tr.size());
        if (vertex_buf == nullptr)
        {
            engine.log << "can't create vertex buffer\n";
            return;
        }
    }

    snd = engine.create_sound("t2_no_problemo.wav");
}

void tanks_game::on_event(om::event& event)
{
    std::cout << event << std::endl;
    switch (event.type)
    {
        case om::event_type::hardware:
            if (std::get<om::hardware_data>(event.info).is_reset)
            {
                engine.exit(EXIT_SUCCESS);
            }
            break;
        case om::event_type::input_key:
        {
            const auto& key_data = std::get<om::input_data>(event.info);
            if (key_data.is_down)
            {
                if (key_data.key == om::keys::button1)
                {
                    snd->play(om::sound::effect::once);
                }
                else if (key_data.key == om::keys::button2)
                {
                    snd->play(om::sound::effect::looped);
                }
            }
        }
        break;
    }
}

void tanks_game::on_update(std::chrono::milliseconds /*frame_delta*/)
{
    if (engine.is_key_down(om::keys::left))
    {
        current_tank_pos.x -= 0.01f;
        current_tank_direction = -pi / 2.f;
    }
    else if (engine.is_key_down(om::keys::right))
    {
        current_tank_pos.x += 0.01f;
        current_tank_direction = pi / 2.f;
    }
    else if (engine.is_key_down(om::keys::up))
    {
        current_tank_pos.y += 0.01f;
        current_tank_direction = 0.f;
    }
    else if (engine.is_key_down(om::keys::down))
    {
        current_tank_pos.y -= 0.01f;
        current_tank_direction = -pi;
    }
}

void tanks_game::on_render() const
{
    om::mat2x3 move   = om::mat2x3::move(current_tank_pos);
    om::mat2x3 aspect = om::mat2x3::scale(1, 640.f / 480.f);
    om::mat2x3 rot    = om::mat2x3::rotation(current_tank_direction);
    om::mat2x3 m      = rot * move * aspect;

    engine.render(*vertex_buf, texture, m);
}

int initialize_and_start_main_loop()
{
    om::engine engine("");

    std::unique_ptr<om::lila> game = om_tat_sat(engine);

    game->on_initialize();

    while (true)
    {
        om::event event;

        while (engine.read_event(event))
        {
            game->on_event(event);
        }

        game->on_update(std::chrono::milliseconds(1));
        game->on_render();

        engine.swap_buffers();
    }

    return EXIT_SUCCESS;
}

int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        return initialize_and_start_main_loop();
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    return EXIT_FAILURE;
}
