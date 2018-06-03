#include "fruit.hxx"

void fruit::generate_next_position(const std::vector<uint32_t>& free_cells)
{
    const int32_t cell_index = free_cells.at(rand() % free_cells.size());
    sprite.position.x        = (cell_index % 28) * 10 - 140 + 5;
    sprite.position.y        = (cell_index / 28) * 10 - 140 + 5;
}
