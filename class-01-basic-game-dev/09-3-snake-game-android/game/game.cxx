#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <ranges>
#include <string_view>
#include <vector>

#include <om/engine.hxx>

#include "configuration_loader.hxx"
#include "fruit.hxx"
#include "game_object.hxx"
#include "snake.hxx"

size_t       screen_width  = 960.f;
size_t       screen_height = 540.f;
om::texture* debug_texture = nullptr;

class snake_game final : public om::lila
{
public:
    snake_game() = default;

    void on_initialize() final;
    void on_event(om::event&) final;
    void on_update(std::chrono::milliseconds frame_delta) final;
    void on_render() const final;

private:
    std::vector<game_object>            objects;
    std::map<std::string, om::vbo*>     meshes;
    std::map<std::string, om::texture*> textures;
    std::unique_ptr<snake>              snake_;
    std::unique_ptr<fruit>              fruit_;
    std::vector<uint32_t>               free_cells;
    std::vector<bool>                   cell_state;

    void update_free_cells();
};

std::unique_ptr<om::lila> om_tat_sat()
{
    om::log << "initialize engine" << std::endl;

    om::window_mode window_mode = { screen_width, screen_height, false };
    om::initialize("snake", window_mode);

    om::log << "creating main game object..." << std::endl;
    auto game = std::make_unique<snake_game>();
    om::log << "finish creating main game object" << std::endl;
    return game;
}

om::vbo* load_mesh_from_file_with_scale(const std::string_view path,
                                        const om::vec2&        scale);

void snake_game::update_free_cells()
{
    cell_state.clear();
    cell_state.resize(28 * 28);
    snake_->fill_cells(cell_state);
    free_cells.clear();

    auto is_cell_empty = [this](uint32_t i) { return !cell_state.at(i); };

    using namespace std;
    using namespace std::views;

    auto empty_cells = iota(0u, 28u * 28u) | filter(is_cell_empty);

    copy(begin(empty_cells), end(empty_cells), back_inserter(free_cells));

    /*
        for (uint32_t i = 0; i < 28 * 28; ++i)
        {
            if (!cell_state.at(i))
            {
                free_cells.push_back(i);
            }
        }
    */
}

void snake_game::on_initialize()
{
    om::get_window_size(screen_width, screen_height);
    free_cells.reserve(28 * 28);

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

    snake_.reset(
        new ::snake(om::vec2(35, 5), snake::direction::right, objects));

    fruit_.reset(new fruit());
    fruit_->sprite = objects.at(1);

    update_free_cells();
    fruit_->generate_next_position(free_cells);
}

void snake_game::on_event(om::event& event)
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
                else if (key_data.key == om::keys::left)
                {
                    snake_->set_user_direction(snake::user_direction::left);
                }
                else if (key_data.key == om::keys::right)
                {
                    snake_->set_user_direction(snake::user_direction::right);
                }
            }
        }
        break;
    }
}

void snake_game::on_update(std::chrono::milliseconds frame_delta)
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

    if (snake_)
    {
        // TODO check collision snake with apple
        // 1. get apple position
        om::vec2 fruit_pos = fruit_->sprite.position;
        // 2. get snake head position
        om::vec2 head_pos = snake_->parts.front().game_obj.position;
        // 3. compare cell positions
        om::vec2 distance = fruit_pos - head_pos;
        if (distance.length() <= 5)
        {
            // TODO generate next fruit position
            update_free_cells();
            fruit_->generate_next_position(free_cells);

            snake_->eat_fruit();
        }
        // 4. if same - add one snake_part

        float dt = frame_delta.count() * 0.001f;
        snake_->update(dt);
    }
}

void snake_game::on_render() const
{
    struct draw
    {
        draw(const object_type type, const om::vec2& world_size)
            : obj_type(type)
            , world(om::matrix::scale(2.f / world_size.x, 2.f / world_size.y))
        {
            // let the world is rectangle 100x100 units with center in (0, 0)
            // build world matrix to map 100x100 X(-50 to 50) Y(-50 to 50)
            // to NDC X(-1 to 1) Y(-1 to 1) 2x2
        }
        void operator()(const game_object& obj)
        {
            if (obj_type == obj.type)
            {
                om::matrix aspect = om::matrix::scale(
                    static_cast<float>(screen_height) / screen_width, 1);

                om::matrix move = om::matrix::move(obj.position);
                om::matrix rot  = om::matrix::rotation(obj.rotation);
                om::matrix tmp  = rot * move * world;
                om::matrix m    = tmp * aspect;

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

    static const std::vector<object_type> render_order = { object_type::level };

    auto it =
        std::find_if(begin(objects), end(objects), [](const game_object& obj) {
            return obj.type == object_type::level;
        });

    if (it == end(objects))
    {
        throw std::runtime_error("no level object");
    }

    const om::vec2 world_size = it->size;

    std::for_each(
        begin(render_order), end(render_order), [&](object_type type) {
            std::for_each(begin(objects), end(objects), draw(type, world_size));
        });

    if (fruit_)
    {
        draw renderer(object_type::fruit, world_size);
        renderer(fruit_->sprite);
    }

    if (snake_)
    {
        const std::vector<game_object*>& render_list = snake_->render_list();
        for (game_object* obj : render_list)
        {
            draw renderer(object_type::snake_part, world_size);
            renderer(*obj);
        }
    }
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
