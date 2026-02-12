#include "fps_camera.hxx"

#include <SDL3/SDL.h>

void fps_camera::move_using_keyboard_wasd(const float delta_time)
{
    const bool* keys_state = SDL_GetKeyboardState(nullptr);
    if (keys_state[SDL_SCANCODE_W])
    {
        move(fps_camera::step::forward, delta_time);
    }
    if (keys_state[SDL_SCANCODE_S])
    {
        move(fps_camera::step::backward, delta_time);
    }
    if (keys_state[SDL_SCANCODE_A])
    {
        move(fps_camera::step::left, delta_time);
    }
    if (keys_state[SDL_SCANCODE_D])
    {
        move(fps_camera::step::right, delta_time);
    }
}

glm::vec3 fps_camera::position() const
{
    return position_;
}

void fps_camera::position(const glm::vec3& value)
{
    position_ = value;
}
