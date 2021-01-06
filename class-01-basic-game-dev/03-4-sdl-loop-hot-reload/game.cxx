#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string_view>

#include "engine.hxx"

class my_concole_game : public om::game
{
public:
    explicit my_concole_game(om::engine&)
        : rotation_index{ 0 }
        , rotations_chars{ { '-', '/', '|', '\\' } }
    {
    }
    void initialize() override {}
    void on_event(om::event) override {}
    void update() override
    {
        ++rotation_index;
        rotation_index %= rotations_chars.size();
    }
    void render() const override
    {
        const char current_symbol = rotations_chars.at(rotation_index);
        std::cout << '\b' << current_symbol << std::flush;
    }

private:
    uint32_t                  rotation_index = 0;
    const std::array<char, 4> rotations_chars;
};

/// We have to export next two functions
OM_DECLSPEC om::game* create_game(om::engine* engine)
{
    if (engine != nullptr)
    {
        return new my_concole_game(*engine);
    }
    return nullptr;
}

OM_DECLSPEC void destroy_game(om::game* game)
{
    delete game;
}
