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

#include "configuration_loader.hxx"

static constexpr size_t screen_width  = 960.f;
static constexpr size_t screen_height = 540.f;

class tanks_game final : public om::lila
{
public:
    tanks_game() = default;

    void on_initialize() final;
    void on_event(om::event&) final;
    void on_update(std::chrono::milliseconds frame_delta) final;
    void on_render() const final;

private:
    om::texture* texture    = nullptr;
    om::vbo*     vertex_buf = nullptr;
    om::sound*   snd        = nullptr;

    om::vec2               current_tank_pos       = om::vec2(0.f, 0.f);
    float                  current_tank_direction = 0.f;
    static constexpr float pi                     = 3.1415926f;
};

std::unique_ptr<om::lila> om_tat_sat()
{
    om::log << "initialize engine" << std::endl;

    om::window_mode window_mode = { screen_width, screen_height, false };
    om::initialize("tanks", window_mode);

    om::log << "creating main game object..." << std::endl;
    auto game = std::make_unique<tanks_game>();
    om::log << "finish creating main game object" << std::endl;
    return game;
}

void tanks_game::on_initialize()
{
    texture =
        om::create_texture("01-basic-game-dev/08-3-engine-cleanup/tank.png");
    if (nullptr == texture)
    {
        om::log << "failed load texture\n";
        return;
    }

    vertex_buf = nullptr;

    std::stringstream file = filter_comments(
        "01-basic-game-dev/08-3-engine-cleanup/vert_tex_color.txt");
    if (!file)
    {
        om::log << "can't load vert_tex_color.txt\n";
        return;
    }
    else
    {
        std::array<om::vertex, 6> vertexes;
        for (size_t i = 0; i < vertexes.size(); ++i)
        {
            file >> vertexes[i];
        }

        vertex_buf = create_vbo(vertexes.data(), vertexes.size());
        if (vertex_buf == nullptr)
        {
            om::log << "can't create vertex buffer\n";
            return;
        }
    }

    snd = om::create_sound(
        "01-basic-game-dev/08-3-engine-cleanup/t2_no_problemo.wav");
}

void tanks_game::on_event(om::event& event)
{
    std::cout << event << std::endl;
    switch (event.type)
    {
        case om::event_type::hardware:
            if (std::get<om::hardware_data>(event.info).is_reset)
            {
                om::exit(EXIT_SUCCESS);
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
    if (om::is_key_down(om::keys::left))
    {
        current_tank_pos.x -= 0.01f;
        current_tank_direction = -pi / 2.f;
    }
    else if (om::is_key_down(om::keys::right))
    {
        current_tank_pos.x += 0.01f;
        current_tank_direction = pi / 2.f;
    }
    else if (om::is_key_down(om::keys::up))
    {
        current_tank_pos.y += 0.01f;
        current_tank_direction = 0.f;
    }
    else if (om::is_key_down(om::keys::down))
    {
        current_tank_pos.y -= 0.01f;
        current_tank_direction = -pi;
    }
}

void tanks_game::on_render() const
{
    om::matrix move = om::matrix::move(current_tank_pos);
    om::matrix aspect =
        om::matrix::scale(1, static_cast<float>(screen_width) / screen_height);
    om::matrix rot = om::matrix::rotation(current_tank_direction);
    om::matrix m   = rot * move * aspect;

    om::render(om::primitives::triangls, *vertex_buf, texture, m);
}
