#include "snake.hxx"
#include "game_object.hxx"

snake::snake(om::vec2 pos, snake::direction head_direction,
             const std::vector<game_object>& sprites)
{
    snake_part head;
    head.dir           = head_direction;
    game_object sprite = sprites.at(8);
    sprite.position    = pos;
    head.game_obj      = sprite;
    parts.push_back(head);

    snake_part body;
    body.dir        = head_direction;
    sprite          = sprites.at(3);
    sprite.position = pos + om::vec2(-10, 0);
    body.game_obj   = sprite;
    parts.push_back(body);

    snake_part tail;
    tail.dir        = head_direction;
    sprite          = sprites.at(2);
    sprite.position = pos + om::vec2(-20, 0);
    tail.game_obj   = sprite;
    parts.push_back(tail);

    render_list_.reserve(28 * 28);
}

void snake::update(float /*dt*/)
{
    // TODO ...
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
