#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string_view>
#include <vector>

#pragma GCC diagnostic push
// turn off the specific warning. Can also use "-Wall"
#pragma GCC diagnostic ignored "-Wall"
#include "imgui.h"
#pragma GCC diagnostic pop

#include "engine.hxx"
#include "sprite.hxx"
#include "sprite_reader.hxx"

#include "ani2d.hxx"

#include <chrono> // for std::chrono functions

class timer
{
public:
    timer()
        : m_beg(clock_t::now())
    {
    }

    void reset() { m_beg = clock_t::now(); }

    float elapsed() const
    {
        return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg)
            .count();
    }

private:
    // Type aliases to make accessing nested type easier
    using clock_t  = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<float, std::ratio<1>>;

    std::chrono::time_point<clock_t> m_beg;
};

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;
    unique_ptr<om::engine, void (*)(om::engine*)> e(om::create_engine(),
                                                    om::destroy_engine);

    if (!e)
    {
        cerr << "can't create engine object";
        return EXIT_FAILURE;
    }

    const string error = e->initialize("");
    if (!error.empty())
    {
        cerr << error << endl;
        return EXIT_FAILURE;
    }

    om::engine& engine = *e;

    bool continue_loop = true;

    [[maybe_unused]] constexpr float pi = 3.1415926f;

    string       texture_path(1024, '\0');
    string       texture_cache_file(1024, '\0');
    string       sprite_id(1024, '\0');
    om::texture* texture        = nullptr;
    rect         spr_rect       = {};
    om::vec2     spr_center_pos = {};
    om::vec2     spr_size       = {};
    float        angle          = 0.f;

    sprite_reader  loader_of_sprites;
    vector<sprite> sprites_for_animation;
    ifstream       ifile;
    ifile.exceptions(std::ios::badbit | std::ios::failbit);
    ifile.open("spr_cache.yaml", ios::binary);
    loader_of_sprites.load_sprites(sprites_for_animation, ifile, engine);

    ani2d animation;
    animation.sprites(sprites_for_animation);
    animation.fps(2);

    om::vec2 mouse_pos = engine.mouse_pos();
    om::vec2 image_screen_start_pos;

    [[maybe_unused]] bool is_left_mouse_holded = false;
    om::vec2              start_drag_pos;
    om::vec2              end_drag_pos;

    timer timer_;
    while (continue_loop)
    {
        [[maybe_unused]] float delta_time = timer_.elapsed();
        timer_.reset();

        om::event event;

        while (engine.read_event(event))
        {
            cout << event << endl;
            switch (event)
            {
                case om::event::turn_off:
                    continue_loop = false;
                    break;
                case om::event::left_mouse_pressed:
                {
                    om::vec2 cur_pos = engine.mouse_pos();
                    if (cur_pos.x >= image_screen_start_pos.x &&
                        cur_pos.y >= image_screen_start_pos.y &&
                        texture != nullptr &&
                        cur_pos.x <=
                            image_screen_start_pos.x + texture->get_width() &&
                        cur_pos.y <=
                            image_screen_start_pos.y + texture->get_height())
                    {
                        is_left_mouse_holded = true;
                        start_drag_pos       = cur_pos;
                    }
                }
                break;
                case om::event::left_mouse_released:
                    if (is_left_mouse_holded)
                    {
                        is_left_mouse_holded = false;
                        end_drag_pos         = engine.mouse_pos();
                    }
                    break;
                case om::event::mouse_moved:
                    if (is_left_mouse_holded)
                    {
                        end_drag_pos = engine.mouse_pos();
                    }
                    break;
                default:

                    break;
            }
        }

        mouse_pos = engine.mouse_pos();

        om::mat2x3 move        = om::mat2x3::move(om::vec2(0.f, 0.f));
        om::vec2   screen_size = engine.screen_size();
        om::mat2x3 aspect = om::mat2x3::scale(1, screen_size.x / screen_size.y);
        // om::mat2x3 rot    = om::mat2x3::rotation(current_tank_direction);
        [[maybe_unused]] om::mat2x3 m = /*rot **/ move * aspect;

        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(screen_size.x, 0));

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
                stringstream message;
                message << "Now you move over image, and hold LEFT mouse in \n"
                           "top left angle you want sprite has, then move to \n"
                           "bottom right and unpress LEFT mouse"
                        << "[" << setw(3)
                        << mouse_pos.x - image_screen_start_pos.x << ", "
                        << setw(3) << mouse_pos.y - image_screen_start_pos.y
                        << "]\n"
                        << "selection start [" << setw(3) << start_drag_pos.x
                        << ", " << setw(3) << start_drag_pos.y << "]"
                        << "selection end [" << setw(3) << end_drag_pos.x
                        << ", " << setw(3) << end_drag_pos.y << "]";
                string msg = message.str();
                ImGui::TextUnformatted(msg.data(), msg.data() + msg.size());

                // get next screen position for image control
                // it is not mouse cursor, but it is virtual next render pos
                ImVec2 cur_screen_pos    = ImGui::GetCursorScreenPos();
                image_screen_start_pos.x = cur_screen_pos.x;
                image_screen_start_pos.y = cur_screen_pos.y;

                ImGui::Image(
                    texture,
                    ImVec2(texture->get_width(), texture->get_height()),
                    ImVec2(0, 1), ImVec2(1, 0));

                if (start_drag_pos.x >= image_screen_start_pos.x &&
                    start_drag_pos.y >= image_screen_start_pos.y &&
                    end_drag_pos.x > start_drag_pos.x &&
                    end_drag_pos.y > start_drag_pos.y &&
                    end_drag_pos.x <=
                        image_screen_start_pos.x + texture->get_width() &&
                    end_drag_pos.y <=
                        image_screen_start_pos.y + texture->get_height())
                {
                    // our selection is inside image so we can render
                    // selection over it
                    ImVec2 p(start_drag_pos.x, start_drag_pos.y);
                    float  sel_w = end_drag_pos.x - start_drag_pos.x;
                    float  sel_h = end_drag_pos.y - start_drag_pos.y;
                    ImGui::GetWindowDrawList()->AddLine(
                        p, ImVec2(p.x + sel_w, p.y), IM_COL32(255, 0, 0, 255),
                        3.0f);
                    ImGui::GetWindowDrawList()->AddLine(
                        p, ImVec2(p.x, p.y + sel_h), IM_COL32(255, 0, 0, 255),
                        3.0f);
                    ImGui::GetWindowDrawList()->AddLine(
                        ImVec2(p.x + sel_w, p.y),
                        ImVec2(p.x + sel_w, p.y + sel_h),
                        IM_COL32(255, 0, 0, 255), 3.0f);
                    ImGui::GetWindowDrawList()->AddLine(
                        ImVec2(p.x, p.y + sel_h),
                        ImVec2(p.x + sel_w, p.y + sel_h),
                        IM_COL32(255, 0, 0, 255), 3.0f);

                    // update rect
                    if (is_left_mouse_holded)
                    {
                        spr_rect.pos.x =
                            start_drag_pos.x - image_screen_start_pos.x;
                        spr_rect.pos.y =
                            start_drag_pos.y - image_screen_start_pos.y;
                        spr_rect.size.x = sel_w;
                        spr_rect.size.y = sel_h;
                    }
                }

                if (ImGui::InputFloat4("uv_rect", &spr_rect.pos.x))
                {
                    // update selection
                    start_drag_pos.x =
                        spr_rect.pos.x + image_screen_start_pos.x;
                    start_drag_pos.y =
                        spr_rect.pos.y + image_screen_start_pos.y;
                    end_drag_pos.x = start_drag_pos.x + spr_rect.size.x;
                    end_drag_pos.y = start_drag_pos.y + spr_rect.size.y;
                }
                ImGui::InputFloat2("world_pos", &spr_center_pos.x);
                ImGui::InputFloat2("size", &spr_size.x);
                ImGui::SliderFloat("angle", &angle, 0.0f, 360.f);

                if (spr_rect.size.length() > 0.f && spr_size.length() > 0.f &&
                    strlen(texture_cache_file.data()) > 0)
                {
                    if (ImGui::Button("save to cache"))
                    {
                        vector<sprite> sprites;
                        sprites.emplace_back(sprite_id, texture, spr_rect,
                                             spr_center_pos, spr_size, angle);

                        sprite_reader reader;
                        ofstream      fout;
                        fout.exceptions(ios::badbit);
                        fout.open(texture_cache_file.data(), ios::binary);
                        reader.save_sprites(sprites, fout);
                    }
                }
            }
        } // end window "sprite properties"
        ImGui::End();

        // Rendering
        ImGui::Render();

        rect from_pixels_to_relative = spr_rect;

        if (texture != nullptr)
        {
            from_pixels_to_relative.pos.x /= texture->get_width();
            from_pixels_to_relative.pos.y /= texture->get_height();
            from_pixels_to_relative.size.x /= texture->get_width();
            from_pixels_to_relative.size.y /= texture->get_height();
        }

        sprite spr(sprite_id, texture, from_pixels_to_relative, spr_center_pos,
                   spr_size, angle);

        spr.draw(engine);

        animation.draw(engine, delta_time);

        engine.swap_buffers();
    }

    engine.uninitialize();

    return EXIT_SUCCESS;
}
