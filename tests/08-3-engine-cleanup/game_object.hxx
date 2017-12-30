#include <sstream>
#include <string>

enum class object_type
{
    level,
    ai_tank,
    user_tank,
    brick_wall
};

struct game_object
{
    std::string name;
};
