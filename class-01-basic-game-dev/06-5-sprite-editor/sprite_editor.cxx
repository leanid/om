#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

#include "imgui.h"

#include "engine.hxx"
#include "sprite.hxx"

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

    om::texture* texture = engine->create_texture("tank.png");
    if (nullptr == texture)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }

    om::vertex_buffer* vertex_buf = nullptr;

    std::ifstream file("vert_tex_color.txt");
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

    bool continue_loop = true;

    om::vec2    current_tank_pos(0.f, 0.f);
    float       current_tank_direction(0.f);
    const float pi = 3.1415926f;

    std::string texture_path;
    texture_path.reserve(1024);
    om::texture* loaded_tex     = nullptr;
    rect         spr_rect       = {};
    om::vec2     spr_center_pos = {};
    om::vec2     spr_size       = {};
    float        angle          = 0.f;

    while (continue_loop)
    {
        om::event event;

        while (engine->read_event(event))
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

        if (engine->is_key_down(om::keys::left))
        {
            current_tank_pos.x -= 0.01f;
            current_tank_direction = pi / 2.f;
        }
        else if (engine->is_key_down(om::keys::right))
        {
            current_tank_pos.x += 0.01f;
            current_tank_direction = -pi / 2.f;
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

        // Start the frame. This call will update the io.WantCaptureMouse,
        // io.WantCaptureKeyboard flag that you can use to dispatch inputs (or
        // not) to your application.
        ImGui::NewFrame();

        bool show_demo_window = false;
        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        ImGui::InputText("texture: ", texture_path.data(),
                         texture_path.capacity());

        if (ImGui::Button("load texture"))
        {
            if (texture != nullptr)
            {
                engine->destroy_texture(loaded_tex);
            }
            loaded_tex = engine->create_texture(texture_path);
        }

        if (loaded_tex != nullptr)
        {
            ImGui::Image(texture,
                         ImVec2(texture->get_width(), texture->get_height()));
        }

        ImGui::InputFloat4("uv_rect", &spr_rect.pos.x);
        ImGui::InputFloat2("world_pos", &spr_center_pos.x);
        ImGui::InputFloat2("size", &spr_size.x);
        ImGui::InputFloat("angle", &angle);

        // Rendering
        ImGui::Render();

        sprite spr(texture, spr_rect, spr_center_pos, spr_size, angle);

        spr.draw(*engine);

        engine->swap_buffers();
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
