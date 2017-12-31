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

#include "configuration_loader.hxx"
#include "game_object.hxx"

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
    std::vector<game_object> objects;
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
void tanks_game::on_initialize()
{
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
            it_mesh->second = mesh;
            assert(mesh);
            obj.mesh = mesh;
        }
        auto it_tex = textures.find(obj.path_texture);
        if (it_tex == end(textures))
        {
            om::texture* tex = om::create_texture(obj.path_texture);
            it_tex->second   = tex;
            assert(tex);
            obj.texture = tex;
        }
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

void tanks_game::on_render() const
{
    struct draw
    {
        draw(object_type type)
            : obj_type(type)
            , world(om::matrix::scale(0.01f, 0.01f))
        {
            // let the world is rectangle 100x100 units with center in (0, 0)
            // build world matrix to map 100x100 X(-50 to 50) Y(-50 to 50)
            // to NDC X(-1 to 1) Y(-1 to 1)
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

                om::vbo&     vbo     = *obj.mesh;
                om::texture* texture = obj.texture;

                om::render(om::primitives::triangls, vbo, texture, m);
            }
        }
        const object_type obj_type;
        const om::matrix  world;
    };

    static const std::vector<object_type> render_order = {
        { object_type::level },
        { object_type::brick_wall },
        { object_type::ai_tank },
        { object_type::user_tank }
    };

    std::for_each(begin(render_order), end(render_order),
                  [&](object_type type) {
                      std::for_each(begin(objects), end(objects), draw(type));
                  });
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
    return vbo;
}
