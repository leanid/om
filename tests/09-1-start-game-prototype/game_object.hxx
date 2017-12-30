#pragma once

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

struct model
{
    om::vbo*     vbo     = nullptr;
    om::texture* texture = nullptr;
};

struct game_object
{
    std::string      name;
    enum object_type type;
    float            direction;
    om::vec2         position;
    om::vec2         size;
    std::string      model_description_path;

    model mesh;
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

    std::string model_key_word;
    stream >> model_key_word;
    if (model_key_word != "model")
    {
        throw std::runtime_error("can't parse game object model got: " +
                                 model_key_word);
    }

    stream >> obj.model_description_path;
    return stream;
}

std::istream& operator>>(std::istream& stream, object_type& type)
{
    std::string type_name;
    stream >> type_name;
    if (type_name == "level")
    {
        type = object_type::level;
    }
    else if (type_name == "ai_tank")
    {
        type = object_type::ai_tank;
    }
    else if (type_name == "user_tank")
    {
        type = object_type::user_tank;
    }
    else if (type_name == "brick_wall")
    {
        type = object_type::brick_wall;
    }
    else
    {
        throw std::runtime_error("can't load object with type: " + type_name);
    }
    return stream;
}
