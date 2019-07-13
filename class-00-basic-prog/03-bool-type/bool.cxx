#include "bool.hxx"

namespace om
{
static std::byte false_byte{ 0 };
static std::byte true_byte{ 1 };

const bool_ true_{ reinterpret_cast<bool_&>() };
const bool_ false_;
} // namespace om
