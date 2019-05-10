#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class fps_camera
{
public:
    fps_camera(glm::vec3 a_position, glm::vec3 a_direction, glm::vec3 a_up);
    fps_camera()                  = default;
    fps_camera(const fps_camera&) = default;
    fps_camera& operator=(const fps_camera&) = default;

    glm::mat4 view_matrix() const;

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void update_camera_vectors()
    {
        using namespace std;
        using namespace glm;
        // Calculate the new Front vector
        vec3  front_;
        float yaw_rad   = radians(yaw);
        float pitch_rad = radians(pitch);
        front_.x        = sin(yaw_rad);
        front_.y        = sin(pitch_rad);
        front_.z        = cos(pitch_rad) * cos(yaw_rad);

        front = normalize(front_);
        // Also re-calculate the Right and Up vector
        right = normalize(cross(
            front, world_up)); // Normalize the vectors, because their length
                               // gets closer to 0 the more you look up or down
                               // which results in slower movement.
        up = normalize(cross(right, front));
    }

    // camera attributes
    glm::vec3 position;
    glm::vec3 front{ 0.f, 0.f, -1.f };
    glm::vec3 up{ 0.f, 1.f, 0.f };
    glm::vec3 right;
    glm::vec3 world_up{ 0.f, 1.f, 0.f };
    // Euler angles
    float yaw{ -90.f };
    float pitch{ 0.f };
    // camera options
    float movement_speed{ 3.0f };
    float mouse_sensitivity{ 0.1f };
    float zoom{ 45.f };
};

inline glm::mat4 fps_camera::view_matrix() const
{
    return glm::lookAt(position, position + front, up);
}

inline fps_camera::fps_camera(glm::vec3 a_position, glm::vec3 a_direction,
                              glm::vec3 a_up)
{
    a_direction = glm::normalize(a_direction);
    // if direction {0, 0, -1} expect yaw == 90
    float yaw_rad = std::asin(a_direction.x);
    yaw           = glm::degrees(yaw_rad);
    // if direction {0, 0, -1} expect pitch == 0
    float pitch_rad = std::asin(a_direction.y);
    pitch           = glm::degrees(pitch_rad);

    position = a_position;
    world_up = glm::normalize(a_up);

    update_camera_vectors();
}
