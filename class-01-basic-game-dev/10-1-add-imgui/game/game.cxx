#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string_view>
#include <vector>

#include <om/engine.hxx>
#include <om/imgui.h>

#include "configuration_loader.hxx"
#include "game_object.hxx"

static constexpr size_t screen_width  = 960.f;
static constexpr size_t screen_height = 540.f;
om::texture*            debug_texture = nullptr;

class tanks_game final : public om::lila
{
public:
    tanks_game() = default;

    void on_initialize() final;
    void on_event(om::event&) final;
    void on_update(std::chrono::milliseconds frame_delta) final;
    void on_render() const final;

private:
    std::vector<game_object>            objects;
    std::map<std::string, om::vbo*>     meshes;
    std::map<std::string, om::texture*> textures;
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

om::vbo* load_mesh_from_file_with_scale(const std::string_view path,
                                        const om::vec2&        scale);
void     tanks_game::on_initialize()
{
    debug_texture = om::create_texture("res/debug.png");

    auto level = filter_comments("res/level_01.txt");

    std::string num_of_objects;
    level >> num_of_objects;

    if (num_of_objects != "num_of_objects")
    {
        throw std::runtime_error("no num_of_objects in level file");
    }

    size_t objects_num = 0;

    level >> objects_num;

    std::copy_n(std::istream_iterator<game_object>(level), objects_num,
                std::back_inserter(objects));

    std::for_each(begin(objects), end(objects), [&](game_object& obj) {
        auto it_mesh = meshes.find(obj.path_mesh);
        if (it_mesh == end(meshes))
        {
            om::vbo* mesh =
                load_mesh_from_file_with_scale(obj.path_mesh, obj.size);
            assert(mesh);

            meshes[obj.path_mesh] = mesh;
            it_mesh               = meshes.find(obj.path_mesh);
            assert(it_mesh != end(meshes));
        }
        obj.mesh = it_mesh->second;

        auto it_tex = textures.find(obj.path_texture);
        if (it_tex == end(textures))
        {
            om::texture* tex = om::create_texture(obj.path_texture);
            assert(tex);

            textures[obj.path_texture] = tex;
            it_tex                     = textures.find(obj.path_texture);
            assert(it_tex != end(textures));
        }
        obj.texture = it_tex->second;
    });
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
                }
                else if (key_data.key == om::keys::button2)
                {
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
        //        current_tank_pos.x -= 0.01f;
        //        current_tank_direction = -pi / 2.f;
    }
    else if (om::is_key_down(om::keys::right))
    {
        //        current_tank_pos.x += 0.01f;
        //        current_tank_direction = pi / 2.f;
    }
    else if (om::is_key_down(om::keys::up))
    {
        //        current_tank_pos.y += 0.01f;
        //        current_tank_direction = 0.f;
    }
    else if (om::is_key_down(om::keys::down))
    {
        //        current_tank_pos.y -= 0.01f;
        //        current_tank_direction = -pi;
    }
}

// this function implemented in engine
// example for ImGui usage from game
namespace ImGui
{
void ShowDemoWindow(bool* p_open);
}

void tanks_game::on_render() const
{
    struct draw
    {
        draw(const object_type type, const om::vec2& world_size,
             const float height_aspect)
            : obj_type(type)
            , world(om::matrix::scale(2 * height_aspect / world_size.x,
                                      2 * height_aspect / world_size.y))
        {
            // let the world is rectangle 100x100 units with center in (0, 0)
            // build world matrix to map 100x100 X(-50 to 50) Y(-50 to 50)
            // to NDC X(-1 to 1) Y(-1 to 1) 2x2
            // but we see rect with aspect and correct by height
            // so field not 2x2 but aspect_height x aspect_height
        }
        void operator()(const game_object& obj)
        {
            if (obj_type == obj.type)
            {
                om::matrix aspect = om::matrix::scale(
                    1, static_cast<float>(screen_width) / screen_height);

                om::matrix move = om::matrix::move(obj.position);
                om::matrix rot  = om::matrix::rotation(obj.direction);
                om::matrix m    = rot * move * world * aspect;

                assert(obj.mesh);

                om::vbo&     vbo     = *obj.mesh;
                om::texture* texture = obj.texture;

                om::render(om::primitives::triangls, vbo, texture, m);
                if (debug_texture)
                {
                    om::render(om::primitives::line_loop, vbo, debug_texture,
                               m);
                }
            }
        }
        const object_type obj_type;
        const om::matrix  world;
    };

    static const std::vector<object_type> render_order = {
        { object_type::level, object_type::brick_wall, object_type::ai_tank,
          object_type::user_tank }
    };

    auto it =
        std::find_if(begin(objects), end(objects), [](const game_object& obj) {
            return obj.type == object_type::level;
        });

    if (it == end(objects))
    {
        throw std::runtime_error("no level object");
    }

    const om::vec2 world_size = it->size;
    const float    aspect = static_cast<float>(screen_height) / screen_width;

    std::for_each(begin(render_order), end(render_order),
                  [&](object_type type) {
                      std::for_each(begin(objects), end(objects),
                                    draw(type, world_size, aspect));
                  });

    // use default ImGui Demo example
    bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    // try something myself
    bool editor_is_opened = true;
    ImGui::Begin("Level Map Editor", &editor_is_opened);
    ImGui::Text("Hello!");
    ImGui::End();
}

om::vbo* load_mesh_from_file_with_scale(const std::string_view path,
                                        const om::vec2&        scale)
{
    std::stringstream file = filter_comments(path);
    if (!file)
    {
        throw std::runtime_error("can't load vert_tex_color.txt");
    }

    size_t      num_of_vertexes = 0;
    std::string num_of_ver;

    file >> num_of_ver;

    if (num_of_ver != "num_of_vertexes")
    {
        throw std::runtime_error("no key word: num_of_vertexes");
    }

    file >> num_of_vertexes;

    std::vector<om::vertex> vertexes;

    vertexes.reserve(num_of_vertexes);

    std::copy_n(std::istream_iterator<om::vertex>(file), num_of_vertexes,
                std::back_inserter(vertexes));

    om::matrix scale_mat = om::matrix::scale(scale.x, scale.y);

    std::transform(begin(vertexes), end(vertexes), begin(vertexes),
                   [&scale_mat](om::vertex v) {
                       v.pos = v.pos * scale_mat;
                       return v;
                   });

    om::vbo* vbo = om::create_vbo(vertexes.data(), num_of_vertexes);
    assert(vbo);
    return vbo;
}
