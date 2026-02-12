#include "fps_camera.hxx"

#include <SDL3/SDL.h>

bool fps_camera::move_using_keyboard_wasd(const float delta_time)
{
    const bool* keys_state = SDL_GetKeyboardState(nullptr);
    const bool     moved_w    = keys_state[SDL_SCANCODE_W];
    if (moved_w)
    {
        move(fps_camera::step::forward, delta_time);
    }

    const bool moved_s = keys_state[SDL_SCANCODE_S];
    if (moved_s)
    {
        move(fps_camera::step::backward, delta_time);
    }

    const bool moved_a = keys_state[SDL_SCANCODE_A];
    if (moved_a)
    {
        move(fps_camera::step::left, delta_time);
    }

    const bool moved_d = keys_state[SDL_SCANCODE_D];
    if (moved_d)
    {
        move(fps_camera::step::right, delta_time);
    }

    return moved_w || moved_s || moved_a || moved_d;
}

glm::vec3 fps_camera::position() const
{
    return position_;
}

void fps_camera::position(const glm::vec3& value)
{
    position_ = value;
}
