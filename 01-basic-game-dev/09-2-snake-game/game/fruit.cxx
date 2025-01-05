#include "fruit.hxx"

void fruit::generate_next_position(const std::vector<uint32_t>& free_cells)
{
    const auto  rand_index     = static_cast<size_t>(rand());
    const size_t  index_in_range = rand_index % free_cells.size();
    const auto cell_index =
        static_cast<int32_t>(free_cells.at(index_in_range));
    sprite.position.x = (cell_index % 28) * 10 - 140 + 5; // NOLINT
    sprite.position.y = (cell_index / 28) * 10 - 140 + 5; // NOLINT
}
