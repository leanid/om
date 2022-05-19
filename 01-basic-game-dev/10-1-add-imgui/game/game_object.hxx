#pragma once

#include <algorithm>
#include <map>
#include <sstream>
#include <string>

#include "om/math.hxx"

enum class object_type
{
    level,
    ai_tank,
    user_tank,
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

std::istream& operator>>(std::istream& stream, game_object& obj)
{
    std::string start_word;
    stream >> start_word;
    if (start_word != "object")
    {
        throw std::runtime_error("can't parse game object got: " + start_word);
    }

    stream >> obj.name;
    stream >> obj.type;

    float direction_in_grad = 0.0f;
    stream >> direction_in_grad;
    obj.direction = direction_in_grad * (3.1415926f / 180.f);

    stream >> obj.position;
    stream >> obj.size;

    std::string key_word;
    stream >> key_word;
    if (key_word != "mesh")
    {
        throw std::runtime_error("can't parse game object mesh got: " +
                                 key_word);
    }

    stream >> obj.path_mesh;

    stream >> key_word;
    if (key_word != "texture")
    {
        throw std::runtime_error("can't parse game object texture got: " +
                                 key_word);
    }

    stream >> obj.path_texture;

    return stream;
}

std::istream& operator>>(std::istream& stream, object_type& type)
{
    static const std::map<std::string, object_type> types = {
        { "level", object_type::level },
        { "ai_tank", object_type::ai_tank },
        { "user_tank", object_type::user_tank },
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
