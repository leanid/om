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

snake::snake(om::vec2 pos, snake::direction head_direction,
             const std::vector<game_object>& sprites)
    : sprites_(sprites)
{
    snake_part head;
    head.dir           = head_direction;
    game_object sprite = sprites_.at(8);
    sprite.position    = pos;
    head.game_obj      = sprite;
    parts.push_back(head);

    snake_part body;
    body.dir        = head_direction;
    sprite          = sprites_.at(3);
    sprite.position = pos + om::vec2(-10, 0);
    body.game_obj   = sprite;
    parts.push_back(body);

    snake_part tail;
    tail.dir        = head_direction;
    sprite          = sprites_.at(2);
    sprite.position = pos + om::vec2(-20, 0);
    tail.game_obj   = sprite;
    parts.push_back(tail);

    render_list_.reserve(28 * 28);
}

snake::direction snake::get_next_direction(game_object& head)
{
    direction next_direction;
    switch (next_head_direction)
    {
        case user_direction::none:
            next_direction = parts.front().dir;
            break;
        case user_direction::left:
            next_direction = next_left[static_cast<unsigned>(head.direction)];
            break;
        case user_direction::right:
            next_direction = next_right[static_cast<unsigned>(head.direction)];
            break;
    }
    return next_direction;
}

om::texture* snake::select_texture_for_neck(const snake_part& neck)
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

    return sprites_.at(index).texture;
}

void snake::update(float dt)
{
    // TODO ...
    step_timer -= dt;
    if (step_timer <= 0.f)
    {
        step_timer = step_level + step_timer;

        // TODO move snake
        auto get_next_step = [](direction dir) {
            return steps[static_cast<unsigned>(dir)];
        };

        auto&     head           = parts.front().game_obj;
        direction next_direction = get_next_direction(head);

        om::vec2 step          = get_next_step(next_direction);
        om::vec2 next_head_pos = head.position;
        next_head_pos.x += step.x;
        next_head_pos.y += step.y;

        snake_part new_head = parts.front();

        constexpr float                pi = 3.14159f;
        constexpr std::array<float, 4> angles{ pi, pi / 2.0f, 0,
                                               3.f * pi / 2.f };
        new_head.game_obj.direction =
            angles[static_cast<unsigned>(next_direction)];
        new_head.game_obj.position = next_head_pos;
        parts.push_front(new_head);

        // replace previous head sprite with body sprite
        snake_part& neck      = *(++(parts.begin()));
        neck.game_obj.texture = select_texture_for_neck(neck);
        // move tail forward
        snake_part& tail          = *parts.rbegin();
        snake_part& new_tail      = *(++parts.rbegin());
        new_tail.game_obj.texture = tail.game_obj.texture;
        auto it                   = parts.rbegin();
        std::advance(it, 2);
        snake_part& before_new_tail = *it;
        new_tail.game_obj.direction =
            angles[static_cast<unsigned>(before_new_tail.dir)];
        parts.pop_back();
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
