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
static om::texture*     debug_texture = nullptr;

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

    om::window_mode window_mode = { .width         = screen_width,
                                    .heigth        = screen_height,
                                    .is_fullscreen = false };
    om::initialize("tanks", window_mode);

    om::log << "creating main game object..." << std::endl;
    auto game = new tanks_game();
    om::log << "finish creating main game object" << std::endl;
    return std::unique_ptr<om::lila>(game);
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

    std::copy_n(std::istream_iterator<game_object>(level),
                objects_num,
                std::back_inserter(objects));

    std::ranges::for_each(
        objects,

        [&](game_object& obj)
        {
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
                { // NOLINT
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
    { // NOLINT
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
        draw(const object_type type,
             const om::vec2&   world_size,
             const float       height_aspect)
            : obj_type(type)
            , world(om::matrix::scale(2 * height_aspect / world_size.x,
                                      2 * height_aspect / world_size.y))
            , aspect(om::matrix::scale(
                  1, static_cast<float>(screen_width) / screen_height))
            , world_x_aspect(world * aspect)
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

                const om::matrix move = om::matrix::move(obj.position);
                const om::matrix rot  = om::matrix::rotation(obj.direction);
                const om::matrix m    = rot * move * world_x_aspect;

                const om::vbo&     vbo     = *obj.mesh;
                const om::texture* texture = obj.texture;

                om::render(om::primitives::triangls, vbo, texture, m);
                if (debug_texture)
                {
                    om::render(
                        om::primitives::line_loop, vbo, debug_texture, m);
                }
            }
        }
        const object_type obj_type;
        const om::matrix  world;
        const om::matrix  aspect;
        const om::matrix  world_x_aspect;
    };

    static const std::vector<object_type> render_order = {
        { object_type::level,
          object_type::brick_wall,
          object_type::ai_tank,
          object_type::user_tank }
    };

    auto it = std::ranges::find_if(objects,

                                   [](const game_object& obj)
                                   { return obj.type == object_type::level; });

    if (it == end(objects))
    {
        throw std::runtime_error("no level object");
    }

    const om::vec2 world_size = it->size;
    const float    aspect = static_cast<float>(screen_height) / screen_width;

    std::ranges::for_each(render_order,

                          [&](object_type type)
                          {
                              draw draw_op(type, world_size, aspect);
                              std::ranges::for_each(objects, draw_op);
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

    std::copy_n(std::istream_iterator<om::vertex>(file),
                num_of_vertexes,
                std::back_inserter(vertexes));

    om::matrix scale_mat = om::matrix::scale(scale.x, scale.y);

    std::ranges::transform(vertexes,

                           begin(vertexes),
                           [&scale_mat](om::vertex v)
                           {
                               v.pos = v.pos * scale_mat;
                               return v;
                           });

    om::vbo* vbo = om::create_vbo(vertexes.data(), num_of_vertexes);
    return vbo;
}
