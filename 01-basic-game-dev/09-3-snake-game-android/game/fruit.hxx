#pragma once

#include <vector>

#include "game_object.hxx"

struct fruit
{
    game_object sprite;
    void        generate_next_position(const std::vector<uint32_t>& free_cells);
    om::vec2    position() const { return sprite.position; }
};
