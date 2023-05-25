#include "sprite_reader.hxx"

#include <algorithm>
#include <iomanip>
#include <iostream>

void sprite_io::load(std::vector<sprite>& sprites,
                     std::istream&        in,
                     om::engine&          texture_cache)
{
    sprite spr;

    std::string attribute_name;
    // float       value;
    while (!in.eof() && in >> attribute_name)
    {
        if ("uv_rect:" == attribute_name)
        {
            rect r;
            in >> r.pos.x >> r.pos.y >> r.size.x >> r.size.y;
            spr.uv_rect(r);
        }
        else if ("world_pos:" == attribute_name)
        {
            om::vec2 p;
            in >> p.x >> p.y;
            spr.pos(p);
        }
        else if ("size:" == attribute_name)
        {
            om::vec2 s;
            in >> s.x >> s.y;
            spr.size(s);
        }
        else if ("angle:" == attribute_name)
        {
            float a;
            in >> a;
            spr.rotation(a);

            // angle: - last attribute in sprite so add it to result
            sprites.push_back(spr);
        }
        else if ("id:" == attribute_name)
        {
            std::string name;
            in >> name;
            spr.id(name);
        }
        else if ("texture:" == attribute_name)
        {
            std::string name;
            in >> name;
            om::texture* texture = texture_cache.create_texture(name);
            spr.texture(texture);
        }

        in >> std::ws;
    }
}

void sprite_io::save(const std::vector<sprite>& list, std::ostream& ss)
{
    const auto save_one_sprite = [&ss](const sprite& spr)
    {
        using namespace std;
        ss << left << setw(12) << "id: " << spr.id() << '\n';

        const om::texture* texture = spr.texture();
        if (texture == nullptr)
        {
            throw std::runtime_error{ "error: no texture in spite!!!" };
        }

        const std::string_view name = texture->get_name();
        ss << left << setw(12) << "texture: " << name << '\n';
        ss << left << setw(12) << "uv_rect: ";
        const rect& r = spr.uv_rect();
        // clang-format off
        ss << left <<setprecision(3) << fixed
           << setw(7) << r.pos.x << ' '
           << setw(7) << r.pos.y << ' '
           << setw(7) << r.size.x << ' '
           << setw(7) << r.size.y << '\n';
        // clang-format on
        ss << left << setw(12) << "world_pos: " << spr.pos().x << ' '
           << spr.pos().y << '\n';
        ss << left << setw(12) << "size: " << spr.size().x << ' '
           << spr.size().y << '\n';
        ss << left << setw(12) << "angle: " << spr.rotation() << '\n';
    };

    std::for_each(begin(list), end(list), save_one_sprite);
}
