#pragma once

#include <istream>
#include <vector>

#include "sprite.hxx"

namespace sprite_io
{
void load(std::vector<sprite>&, std::istream& in, om::engine& texture_cache);
void save(const std::vector<sprite>&, std::ostream& out);
} // namespace sprite_io
