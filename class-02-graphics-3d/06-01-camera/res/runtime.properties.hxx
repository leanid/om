#include <glm/vec3.hpp>
#include <string>

// you can use this header file directly in your build
// also you can update during runtime this values with
// properties_reader.hxx

bool        enable_depth  = true;
std::string title         = "1-triangles, 2-lines, 3-line-strip, 4-line-loop";
float       z_near        = 0.1f;
float       z_far         = 100.f;
float       fovy          = 45.f;
float       screen_width  = 640.f;
float       screen_height = 480.f;
float       aspect        = screen_width / screen_height;
float       angle         = 60.f;
glm::vec3   rotate_axis   = { 1.f, 0.f, 0.f };
float       angle_per_sec = 10.f;
glm::vec3   move_camera   = { 0.f, 0.f, -5.f };
bool        use_cube      = true;
bool        multi_cube    = true;
float       radius        = 10.f;
glm::vec3   cameraPos     = { 0.0f, 0.0f, 3.0f };
glm::vec3   cameraFront   = { 0.0f, 0.0f, -1.0f };
glm::vec3   cameraUp      = { 0.0f, 1.0f, 0.0f };
float       cameraSpeed   = 5.f;
bool        use_wasd      = true;
float       yaw           = 0.f;
float       pitch         = 0.f;
