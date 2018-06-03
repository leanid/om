#include "snake.hxx"
#include "game_object.hxx"

#include <array>
#include <iterator>

static const std::array<om::vec2, 4> steps{ om::vec2(-10, 0), om::vec2(0, 10),
                                            om::vec2(10, 0), om::vec2(0, -10) };

constexpr std::array<snake::direction, 4> next_left{ snake::direction::down,
                                                     snake::direction::left,
                                                     snake::direction::up,
                                                     snake::direction::right };
constexpr std::array<snake::direction, 4> next_right{ snake::direction::up,
                                                      snake::direction::right,
                                                      snake::direction::down,
                                                      snake::direction::left };

constexpr float                pi = 3.14159f;
constexpr std::array<float, 4> angles{ pi, -pi / 2.0f, 0, -3.f * pi / 2.f };

snake::snake(om::vec2 pos, snake::direction direction,
             const std::vector<game_object>& sprites)
    : sprites_(sprites)
{
    // indexes from res/level_01.txt file
    constexpr unsigned head_index = 8;
    constexpr unsigned body_index = 3;
    constexpr unsigned tail_index = 2;

    auto gen_part = [&](unsigned game_object_index, om::vec2 pos) {
        snake_part part;
        part.dir               = direction;
        part.game_obj          = sprites_.at(game_object_index);
        part.game_obj.position = pos;
        return part;
    };

    snake_part head = gen_part(head_index, pos);
    parts.push_back(head);

    snake_part body = gen_part(body_index, pos + om::vec2(-10, 0));
    parts.push_back(body);

    snake_part tail = gen_part(tail_index, pos + om::vec2(-20, 0));
    parts.push_back(tail);

    render_list_.reserve(28 * 28);
}

snake::direction snake::get_next_direction(const snake_part& head)
{
    direction next_direction;
    switch (next_head_direction)
    {
        case user_direction::none:
            next_direction = parts.front().dir;
            break;
        case user_direction::left:
            next_direction = next_left[static_cast<unsigned>(head.dir)];
            break;
        case user_direction::right:
            next_direction = next_right[static_cast<unsigned>(head.dir)];
            break;
    }
    return next_direction;
}

void snake::select_texture_for_neck(snake_part& neck)
{
    // we have 4 direction (right, up, left, down)
    // for every direction 3 cases (turn_left, turn_right, go_straight)
    // so we have 12 variants
    constexpr uint32_t top_left     = 5;
    constexpr uint32_t top_right    = 6;
    constexpr uint32_t bottom_left  = 4;
    constexpr uint32_t bottom_right = 7;
    constexpr uint32_t body         = 3;

    constexpr std::array<std::array<uint32_t, 3>, 4> texture_indexes{
        { // left
          // {none, left, right}
          { body, bottom_right, top_right },
          // up
          // {node, left, right}
          { body, bottom_left, bottom_right },
          // right
          // {none, left, right}
          { body, top_left, bottom_left },
          // down
          // {node, left, right}
          { body, top_right, top_left } }
    };

    const std::array<uint32_t, 3>& turn_indexes =
        texture_indexes[static_cast<unsigned>(neck.dir)];
    const uint32_t index =
        turn_indexes[static_cast<unsigned>(next_head_direction)];

    neck.game_obj.texture = sprites_.at(index).texture;
    if (index != body)
    {
        neck.game_obj.rotation = 0.f;
    }
}

void snake::remove_old_tail()
{
    parts.pop_back();
}

void snake::update_new_tail()
{
    constexpr float pi        = 3.14159f;
    snake_part&     tail      = *parts.rbegin();
    snake_part&     new_tail  = *(++parts.rbegin());
    new_tail.game_obj.texture = tail.game_obj.texture;
    auto it                   = parts.rbegin();
    std::advance(it, 2);
    snake_part&                    before_new_tail = *it;
    constexpr std::array<float, 4> tail_angles{ pi, -pi / 2.0f, 0, pi / 2.f };
    new_tail.game_obj.rotation =
        tail_angles[static_cast<unsigned>(before_new_tail.dir)];
}

void snake::add_new_head()
{
    snake_part& head           = parts.front();
    direction   next_dir       = get_next_direction(head);
    om::vec2    step           = steps[static_cast<unsigned>(next_dir)];
    snake_part  new_head       = parts.front();
    new_head.game_obj.rotation = angles[static_cast<unsigned>(next_dir)];
    new_head.game_obj.position = head.game_obj.position + step;
    new_head.dir               = next_dir;
    parts.push_front(new_head);
}

void snake::update_old_head()
{
    // TODO show refactoring on done:
    // replace previous head sprite with body sprite
    snake_part& neck = *(++(parts.begin()));
    select_texture_for_neck(neck);
}

void snake::move_snake()
{
    add_new_head();
    update_old_head();
    update_new_tail();
    remove_old_tail();
}

void snake::update(float dt)
{
    // TODO ...
    step_timer -= dt;
    if (step_timer <= 0.f)
    {
        step_timer = step_level + step_timer;

        move_snake();
        // reset next turn
        next_head_direction = user_direction::none;
    }

    render_list_.clear();
    for (auto& part : parts)
    {
        render_list_.push_back(&part.game_obj);
    }
}
const std::vector<game_object*>& snake::render_list() const
{
    return render_list_;
}

void snake::set_user_direction(user_direction next_head_direction)
{
    this->next_head_direction = next_head_direction;
}
bool snake::is_alive() const
{
    return is_alive_;
}
