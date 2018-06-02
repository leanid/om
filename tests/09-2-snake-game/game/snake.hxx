#pragma once

#include <om/math.hxx>

#include <list>
#include <vector>

#include "game_object.hxx"

struct snake
{
    enum class direction
    {
        left,
        up,
        right,
        down
    };

    struct snake_part
    {
        direction   dir;
        game_object game_obj;
    };

    std::list<snake_part>     parts;
    std::vector<game_object*> render_list_;

    enum class user_direction
    {
        none,
        left,
        right
    };
    user_direction next_head_direction = user_direction::none;
    float          step_level          = 1.0f;
    float          step_timer          = 1.0f; // seconds
    bool           is_alive_           = true;

    snake(om::vec2 pos, snake::direction head_direction,
          const std::vector<game_object>& sprites);

    void                             update(float dt);
    const std::vector<game_object*>& render_list() const;
    void set_user_direction(user_direction next_head_direction);
    bool is_alive() const;
};
