#include <glm/vec3.hpp>
#include <string>

// you can use this header file directly in your build
// also you can update during runtime this values with
// properties_reader.hxx

std::string title         = "try 0,1,2,3,4 effect 5,6-cam";
float       z_near        = 0.1f;
float       z_far         = 100.f;
float       fovy          = 45.f;
float       screen_width  = 1024.f;
float       screen_height = 768.f;
float       screen_aspect = screen_width / screen_height;

glm::vec3 cam_pos = { 0.f, 0.f, 3.f };
glm::vec3 cam_dir = { 0.f, 0.f, -1.f };
// to understand normal transform - try rotate cube
float     angle       = 0.000f;
glm::vec3 rotate_axis = { 0.f, 1.f, 0.f };

glm::vec3 clear_color = { 0.0f, 0.0f, 0.0f };

bool show_z_buffer   = false;
bool linear_z_buffer = true;

float     object_scale_outline = 1.05f;
glm::vec3 outline_color        = { 0.04f, 0.28f, 0.26f };

std::string z_buf_operation          = "less";
std::string stensil_operation_sfail  = "keep";
std::string stensil_operation_dpfail = "keep";
std::string stensil_operation_dppass = "replace";
float       stensil_ref_value        = 1.0f;  // static_casted to int in code
float       stensil_mask_value       = 255.f; // static_casted to int 0xFF

bool sort_transparent_quads = true;
