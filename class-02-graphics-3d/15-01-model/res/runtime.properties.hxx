#include <glm/vec3.hpp>
#include <string>

// you can use this header file directly in your build
// also you can update during runtime this values with
// properties_reader.hxx

std::string title  = "1-triangles, 2-lines, 3-line-strip, 4-line-loop, 5,6-cam";
float       z_near = 0.1f;
float       z_far  = 100.f;
float       fovy   = 45.f;
float       screen_width  = 1024.f;
float       screen_height = 768.f;
float       screen_aspect = screen_width / screen_height;

glm::vec3 cam_pos = { 0.f, 10.f, 13.f };
glm::vec3 cam_dir = { 0.f, 0.f, -1.f };
// to understand normal transform - try rotate cube
float     angle       = 0.000f;
glm::vec3 rotate_axis = { 0.f, 1.f, 0.f };

glm::vec3 material_ambient = { 1.0f, 1.0f, 1.0f };
// glm::vec3 material_diffuse   = { 1.0f, 1.0f, 1.0f }; // from texture
// glm::vec3 material_specular  = { 0.5f, 0.5f, 0.5f }; // from texture
float material_shininess = 256.f;

glm::vec3 light_ambient  = { 0.5f, 0.5f, 0.5f };
glm::vec3 light_diffuse  = { 1.0f, 1.0f, 1.0f };
glm::vec3 light_specular = { 1.0f, 1.0f, 1.0f };
glm::vec3 clear_color    = { 0.1f, 0.3f, 0.1f };
