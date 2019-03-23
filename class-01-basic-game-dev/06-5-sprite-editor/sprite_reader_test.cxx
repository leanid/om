
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "sprite_reader.hxx"

#include <iomanip>
#include <iostream>
#include <sstream>

TEST_CASE("test read one sprite", "one")
{
    using namespace std;

    rect r;
    r.pos.x  = 0.30003f;
    r.pos.y  = 0.5f;
    r.size.x = 0.5f;
    r.size.y = 0.5f;
    om::vec2 pos(0.123f, 0.123f);
    om::vec2 size(1.0f, 0.3f);
    float    angle{ 270 };
    string   tank_name{ "tank_spr" };

    stringstream ss;
    ss << "uv_rect: ";
    ss << setprecision(3) << fixed << setw(7);
    ss << r.pos.x << ' ' << r.pos.y << ' ' << r.size.x << ' ' << r.size.y
       << ' ';
    ss << "world_pos: " << pos.x << ' ' << pos.y << ' ';
    ss << "size: " << size.x << ' ' << size.y << ' ';
    ss << "angle: " << angle << ' ';
    ss << "id: " << tank_name;

    sprite_reader  loader;
    vector<sprite> sprites;
    loader.load_sprites(sprites, ss);

    cout << ss.str() << endl << "num_sprites: " << sprites.size() << endl;

    REQUIRE(sprites.size() == 1);
    if (sprites.empty())
    {
        return;
    }
    sprite spr = sprites.at(0);
    REQUIRE(spr.uv_rect().pos.x == r.pos.x);
    REQUIRE(spr.rotation() == angle);
}
