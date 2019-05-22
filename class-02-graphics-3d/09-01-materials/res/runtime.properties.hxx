#include <glm/vec3.hpp>
#include <string>

// you can use this header file directly in your build
// also you can update during runtime this values with
// properties_reader.hxx

std::string title         = "1-triangles, 2-lines, 3-line-strip, 4-line-loop";
float       z_near        = 0.1f;
float       z_far         = 100.f;
float       fovy          = 45.f;
float       screen_width  = 640.f;
float       screen_height = 480.f;
float       aspect        = screen_width / screen_height;
glm::vec3   light_pos     = { -2.f, 1.f, -3.f };
// to understand normal transform - try rotate cube
float     angle       = 0.001f;
glm::vec3 rotate_axis = { 0.f, 1.f, 0.f };

glm::vec3 material_ambient   = { 1.0f, 0.5f, 0.31f };
glm::vec3 material_diffuse   = { 1.0f, 0.5f, 0.31f };
glm::vec3 material_specular  = { 0.5f, 0.5f, 0.5f };
float     material_shininess = 256.f;
