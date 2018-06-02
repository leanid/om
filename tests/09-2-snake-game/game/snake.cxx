#include "snake.hxx"
#include "game_object.hxx"

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

void snake::update(float dt)
{
    // TODO ...
    step_timer -= dt;
    if (step_timer <= 0.f)
    {
        step_timer = step_level + step_timer;

        // TODO move snake
        auto get_next_step = [](direction dir) {
            std::vector<om::vec2> steps{ om::vec2(-10, 0), om::vec2(0, 10),
                                         om::vec2(10, 0), om::vec2(0, -10) };
            return steps[static_cast<unsigned>(dir)];
        };

        auto&     head          = parts.front().game_obj;
        om::vec2  next_head_pos = head.position;
        direction next_direction;
        switch (next_head_direction)
        {
            case user_direction::none:
                next_direction = parts.front().dir;
                break;
            case user_direction::left:
            {
                const std::vector<direction> next_left{ direction::down,
                                                        direction::left,
                                                        direction::up,
                                                        direction::right };

                next_direction =
                    next_left[static_cast<unsigned>(head.direction)];
            }
            break;
            case user_direction::right:
            {
                const std::vector<direction> next_right{ direction::up,
                                                         direction::right,
                                                         direction::down,
                                                         direction::left };

                next_direction =
                    next_right[static_cast<unsigned>(head.direction)];
            }
            break;
        }

        om::vec2 step = get_next_step(next_direction);
        next_head_pos.x += step.x;
        next_head_pos.y += step.y;

        snake_part new_head = parts.front();

        constexpr float    pi = 3.14159f;
        std::vector<float> angles{ pi, pi / 2.0f, 0, 3.f * pi / 2.f };
        new_head.game_obj.direction =
            angles[static_cast<unsigned>(next_direction)];
        new_head.game_obj.position = next_head_pos;
        parts.push_front(new_head);
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
