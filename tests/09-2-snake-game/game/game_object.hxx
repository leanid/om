#pragma once

#include <algorithm>
#include <cassert>
#include <map>
#include <sstream>
#include <string>

#include <om/engine.hxx>

enum class object_type
{
    level,
    fruit,
    snake_part,
    brick_wall
};

struct game_object
{
    std::string      name;
    enum object_type type;
    float            direction;
    om::vec2         position;
    om::vec2         size;
    std::string      path_mesh;
    std::string      path_texture;

    om::vbo*     mesh    = nullptr;
    om::texture* texture = nullptr;
};

inline std::istream& operator>>(std::istream& stream, object_type& type);

om::vbo* load_mesh_from_file_with_scale(const std::string_view path,
                                        const om::vec2&        scale);

std::istream& operator>>(std::istream& stream, game_object& obj);

std::istream& operator>>(std::istream& stream, object_type& type)
{
    static const std::map<std::string, object_type> types = {
        { "level", object_type::level },
        { "fruit", object_type::fruit },
        { "snake_part", object_type::snake_part },
        { "brick_wall", object_type::brick_wall },
    };

    std::string type_name;
    stream >> type_name;

    auto it = types.find(type_name);

    if (it != end(types))
    {
        type = it->second;
    }
    else
    {
        std::stringstream ss;
        ss << "expected one of: ";
        std::for_each(begin(types), end(types),
                      [&ss](auto& kv) { ss << kv.first << ", "; });
        ss << " but got: " << type_name;
        throw std::runtime_error(ss.str());
    }

    return stream;
}
