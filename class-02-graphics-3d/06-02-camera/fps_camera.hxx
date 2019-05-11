#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class fps_camera
{
public:
    enum class step
    {
        forward,
        backward,
        left,
        right
    };
    ///
    /// \brief fps_camera
    /// \param a_position coordinate in world space
    /// \param a_direction vector from camera position to object
    /// \param a_up vector in world space
    ///
    fps_camera(glm::vec3 a_position, glm::vec3 a_direction, glm::vec3 a_up);
    fps_camera()                  = default;
    fps_camera(const fps_camera&) = default;
    fps_camera& operator=(const fps_camera&) = default;

    glm::mat4 view_matrix() const;

    void move(const step direction, const float delta_time);
    ///
    /// \brief rotate
    /// \param delta_yaw in degrees
    /// \param delta_pitch in degrees
    ///
    void rotate(const float delta_yaw, const float delta_pitch);

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
    // In OpenGL camera
    // direction vector pointing from camera position
    // to oposite side from object.
    a_direction *= -1.f;
    // if direction {0, 0, -1} expect yaw == 0 degrees
    float yaw_rad = std::asin(a_direction.x);
    yaw           = glm::degrees(yaw_rad);
    // if direction {0, 0, -1} expect pitch == 0 degrees
    float pitch_rad = std::asin(a_direction.y);
    pitch           = glm::degrees(pitch_rad);

    position = a_position;
    world_up = glm::normalize(a_up);

    update_camera_vectors();
}

inline void fps_camera::move(const step direction, const float delta_time)
{
    const float velocity = movement_speed * delta_time;

    switch (direction)
    {
        case step::forward:
            position += front * velocity;
            break;
        case step::backward:
            position -= front * velocity;
            break;
        case step::left:
            position -= right * velocity;
            break;
        case step::right:
            position += right * velocity;
            break;
    }
}
inline void fps_camera::rotate(const float delta_yaw, const float delta_pitch)
{
    yaw += delta_yaw;
    pitch += delta_pitch;

    update_camera_vectors();
}
