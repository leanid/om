#pragma once

#include "sprite.hxx"

#include <vector>

#include "engine.hxx"

class ani2d final
{
public:
    ani2d();

    void sprites(const std::vector<sprite>& sprites_values)
    {
        sprites_ = sprites_values;
    }

    float fps() const { return fps_; }
    void  fps(float fps_value) { fps_ = fps_value; }

    void restart() { current_time_ = 0.f; }

    void draw(om::engine& e, float delta_time);

private:
    std::vector<sprite> sprites_;
    float               fps_ = 30.f;

    float current_time_ = 0.f;
};
