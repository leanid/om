#pragma once

#include <algorithm>

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
    fps_camera()                             = default;
    fps_camera(const fps_camera&)            = default;
    fps_camera& operator=(const fps_camera&) = default;

    glm::mat4 view_matrix() const;
    glm::mat4 projection_matrix() const;

    void move(const step direction, const float delta_time);
    ///
    /// \brief rotate
    /// \param delta_yaw in degrees
    /// \param delta_pitch in degrees
    ///
    void rotate(const float delta_yaw, const float delta_pitch);

    void zoom(const float zoom);

    void move_using_keyboard_wasd(const float delta_time);

    glm::vec3 position() const;
    void      position(const glm::vec3& value);

    float fovy() const;
    void  fovy(const float);

    float aspect() const;
    void  aspect(const float);

    float z_near() const;
    void  z_near(const float);

    float z_far() const;
    void  z_far(const float);

    ///
    /// \brief direction where camera is looking at
    /// \return normalized verctor
    glm::vec3 direction() const;

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
        // in OpenGL camera look backward to z axis (default NDC coord)
        front_.z = -1.f * cos(pitch_rad) * cos(yaw_rad);

        front = normalize(front_);
        // Also re-calculate the Right and Up vector
        right = normalize(cross(
            front, world_up)); // Normalize the vectors, because their length
                               // gets closer to 0 the more you look up or down
                               // which results in slower movement.
        up = normalize(cross(right, front));
    }

    // camera attributes
    glm::vec3 position_;
    glm::vec3 front{ 0.f, 0.f, -1.f };
    glm::vec3 up{ 0.f, 1.f, 0.f };
    glm::vec3 right;
    glm::vec3 world_up{ 0.f, 1.f, 0.f };
    // projection attributes
    float fovy_{ 45.f };
    float aspect_{ 1.f };
    float z_near_{ 0.1f };
    float z_far_{ 100.f };
    // Euler angles
    float yaw{ 0.f };   /// in degrees
    float pitch{ 0.f }; /// in degrees
    // camera options
    float movement_speed{ 3.0f };
};

inline glm::mat4 fps_camera::view_matrix() const
{
    return glm::lookAt(position_, position_ + front, up);
}

inline glm::mat4 fps_camera::projection_matrix() const
{
    return glm::perspective(glm::radians(fovy_), aspect_, z_near_, z_far_);
}

inline fps_camera::fps_camera(glm::vec3 a_position,
                              glm::vec3 a_direction,
                              glm::vec3 a_up)
{
    a_direction = glm::normalize(a_direction);
    // In OpenGL camera
    // direction vector pointing from camera position
    // to oposite side from object.
    // a_direction = -1.f;
    // if direction {0, 0, -1} expect yaw == 0 degrees
    float yaw_rad = std::asin(a_direction.x);
    yaw           = glm::degrees(yaw_rad);
    // if direction {0, 0, -1} expect pitch == 0 degrees
    float pitch_rad = std::asin(a_direction.y);
    pitch           = glm::degrees(pitch_rad);

    position_ = a_position;
    world_up  = glm::normalize(a_up);

    update_camera_vectors();
}

inline void fps_camera::move(const step direction, const float delta_time)
{
    const float velocity = movement_speed * delta_time;

    switch (direction)
    {
        case step::forward:
            position_ += front * velocity;
            break;
        case step::backward:
            position_ -= front * velocity;
            break;
        case step::left:
            position_ -= right * velocity;
            break;
        case step::right:
            position_ += right * velocity;
            break;
    }
}

inline void fps_camera::rotate(const float delta_yaw, const float delta_pitch)
{
    yaw += delta_yaw;
    pitch += delta_pitch;

    std::clamp(pitch, -89.f, 89.f);

    update_camera_vectors();
}

inline void fps_camera::zoom(const float zoom)
{
    fovy_ += zoom;

    std::clamp(fovy_, 1.f, 45.f);
}

inline float fps_camera::fovy() const
{
    return fovy_;
}
inline void fps_camera::fovy(const float v)
{
    fovy_ = v;
    std::clamp(fovy_, 1.f, 45.f);
}

inline float fps_camera::aspect() const
{
    return aspect_;
}

inline void fps_camera::aspect(const float v)
{
    aspect_ = v;
    std::clamp(aspect_, 0.1f, 10.f);
}

inline float fps_camera::z_near() const
{
    return z_near_;
}

inline void fps_camera::z_near(const float v)
{
    z_near_ = v;
    std::clamp(z_near_, 0.01f, 10000.f);
}

inline float fps_camera::z_far() const
{
    return z_far_;
}

inline void fps_camera::z_far(const float v)
{
    z_far_ = v;
    std::clamp(z_far_, z_near_, 10000.f);
}

inline glm::vec3 fps_camera::direction() const
{
    return glm::normalize(front);
}
