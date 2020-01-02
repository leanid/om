#include <glm/vec3.hpp>
#include <string>

// you can use this header file directly in your build
// also you can update during runtime this values with
// properties_reader.hxx

std::string title         = "1,2,3,4-primitive type, 5,6-cam";
float       z_near        = 0.1f;
float       z_far         = 100.f;
float       fovy          = 45.f;
float       screen_width  = 1024.f;
float       screen_height = 768.f;
float       screen_aspect = screen_width / screen_height;

glm::vec3 cam_pos = { 0.f, 10.f, 13.f };
glm::vec3 cam_dir = { 0.f, 0.f, -1.f };
// to understand normal transform - try rotate cube
float     angle       = 0.000f;
glm::vec3 rotate_axis = { 0.f, 1.f, 0.f };

glm::vec3 nanosuit_material_ambient   = { 0.0f, 0.0f, 0.0f };
float     nanosuit_material_shininess = 256.f;

glm::vec3 dir_light_direction = { 0.0f, 0.0f, -1.0f };
glm::vec3 dir_light_ambient   = { 0.25f, 0.25f, 0.25f };
glm::vec3 dir_light_diffuse   = { 0.6f, 0.6f, 0.6f };
glm::vec3 dir_light_specular  = { 0.5f, 0.5f, 0.5f };

float     point_light_constant  = 1.0f;
glm::vec3 point_light_ambient   = { 0.05f, 0.05f, 0.05f };
glm::vec3 point_light_diffuse   = { 0.8f, 0.8f, 0.8f };
glm::vec3 point_light_specular  = { 1.0f, 1.0f, 1.0f };
float     point_light_linear    = 0.09f;
float     point_light_quadratic = 0.032f;

glm::vec3 spot_light_ambient       = { 0.0f, 0.0f, 0.0f };
glm::vec3 spot_light_diffuse       = { 1.0f, 1.0f, 1.0f };
glm::vec3 spot_light_specular      = { 1.0f, 1.0f, 1.0f };
float     spot_light_constant      = 1.0f;
float     spot_light_linear        = 0.09f;
float     spot_light_quadratic     = 0.032f;
float     spot_light_cut_off       = 12.5f; // degrees
float     spot_light_outer_cut_off = 15.0f; // degrees

glm::vec3 clear_color = { 0.1f, 0.3f, 0.1f };
