#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

#include "imgui.h"

#include "engine.hxx"
#include "sprite.hxx"
#include "sprite_reader.hxx"

int main(int /*argc*/, char* /*argv*/[])
{
    std::unique_ptr<om::engine, void (*)(om::engine*)> e(om::create_engine(),
                                                         om::destroy_engine);

    if (!e)
    {
        std::cerr << "can't create engine object";
        return EXIT_FAILURE;
    }

    const std::string error = e->initialize("");
    if (!error.empty())
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }

    om::engine& engine = *e;

    bool continue_loop = true;

    [[maybe_unused]] constexpr float pi = 3.1415926f;

    std::string  texture_path(1024, '\0');
    std::string  texture_cache_file(1024, '\0');
    std::string  sprite_id(1024, '\0');
    om::texture* texture        = nullptr;
    rect         spr_rect       = {};
    om::vec2     spr_center_pos = {};
    om::vec2     spr_size       = {};
    float        angle          = 0.f;

    while (continue_loop)
    {
        om::event event;

        while (engine.read_event(event))
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

        om::mat2x3 move        = om::mat2x3::move(om::vec2(0.f, 0.f));
        om::vec2   screen_size = engine.screen_size();
        om::mat2x3 aspect = om::mat2x3::scale(1, screen_size.x / screen_size.y);
        // om::mat2x3 rot    = om::mat2x3::rotation(current_tank_direction);
        [[maybe_unused]] om::mat2x3 m = /*rot **/ move * aspect;

        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(500, 0));

        bool is_propetries_window_open = true;
        if (ImGui::Begin("sprite properties", &is_propetries_window_open,
                         ImGuiWindowFlags_NoMove))
        {
            ImGui::InputText("texture_cache_file: ", texture_cache_file.data(),
                             texture_cache_file.size());
            ImGui::InputText("texture: ", texture_path.data(),
                             texture_path.capacity());
            ImGui::InputText("spr_id: ", sprite_id.data(), sprite_id.size());

            if (ImGui::Button("load texture"))
            {
                if (texture != nullptr)
                {
                    engine.destroy_texture(texture);
                }
                texture = engine.create_texture(texture_path.c_str());
            }

            if (texture != nullptr)
            {
                ImGui::Image(texture, ImVec2(texture->get_width(),
                                             texture->get_height()));

                ImGui::InputFloat4("uv_rect", &spr_rect.pos.x);
                ImGui::InputFloat2("world_pos", &spr_center_pos.x);
                ImGui::InputFloat2("size", &spr_size.x);
                ImGui::SliderFloat("angle", &angle, 0.0f, 360.f);

                if (spr_rect.size.length() > 0.f && spr_size.length() > 0.f &&
                    std::strlen(texture_cache_file.data()) > 0)
                {
                    if (ImGui::Button("save to cache"))
                    {
                        std::vector<sprite> sprites;
                        sprites.emplace_back(sprite_id, texture, spr_rect,
                                             spr_center_pos, spr_size, angle);

                        sprite_reader reader;
                        std::ofstream fout;
                        fout.exceptions(std::ios::badbit);
                        fout.open(texture_cache_file.data(), std::ios::binary);
                        reader.save_sprites(sprites, fout);
                    }
                }
            }
        } // end window "sprite properties"
        ImGui::End();

        // Rendering
        ImGui::Render();

        sprite spr(sprite_id, texture, spr_rect, spr_center_pos, spr_size,
                   angle);

        spr.draw(engine);

        engine.swap_buffers();
    }

    engine.uninitialize();

    return EXIT_SUCCESS;
}
