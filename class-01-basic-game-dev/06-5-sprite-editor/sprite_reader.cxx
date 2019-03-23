#include "sprite_reader.hxx"

#include <iostream>

sprite_reader::sprite_reader() {}

void sprite_reader::load_sprites(std::vector<sprite>& sprites, std::istream& in)
{
    sprite spr;

    std::string attribute_name;
    // float       value;
    while (in >> attribute_name)
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
            spr.rotation(a * (3.1415926f / 180));
        }
        else if ("id:" == attribute_name)
        {
            std::string name;
            in >> name;
            spr.id(name);

            // id: - last attribute in sprite so add it to result
            sprites.push_back(spr);
        }
    }
    return;
}

void sprite_reader::save_sprites(const std::vector<sprite>&, std::ostream&)
{
    return;
}
