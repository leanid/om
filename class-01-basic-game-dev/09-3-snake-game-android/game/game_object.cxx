#include "game_object.hxx"

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
    obj.rotation = direction_in_grad * (3.1415926f / 180.f);

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

    om::vbo* mesh = load_mesh_from_file_with_scale(obj.path_mesh, obj.size);
    assert(mesh != nullptr);
    obj.mesh = mesh;

    stream >> key_word;
    if (key_word != "texture")
    {
        throw std::runtime_error("can't parse game object texture got: " +
                                 key_word);
    }

    stream >> obj.path_texture;

    om::texture* tex = om::create_texture(obj.path_texture);
    assert(tex != nullptr);
    obj.texture = tex;

    return stream;
}
