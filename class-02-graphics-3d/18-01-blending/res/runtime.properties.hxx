#include <glm/vec3.hpp>
#include <string>

// you can use this header file directly in your build
// also you can update during runtime this values with
// properties_reader.hxx

std::string title         = "1-show_z_buf,2,3,4-primitive type, 5,6-cam";
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

/// stensil_operations
// { { "keep", GL_KEEP },       /// The currently stored stencil value is kept.
//   { "zero", GL_ZERO },       /// The stencil value is set to 0.
//   { "replace", GL_REPLACE }, /// The stencil value is replaced with ref
//   { "incr", GL_INCR },       /// The stencil value is increased by 1
//   { "incr_wrap", GL_INCR_WRAP }, /// same as above but goto 0
//   { "decr", GL_DECR },           /// decremented by 1
//   { "decr_wrap", GL_DECR_WRAP }, /// same as above but restart with max FF
//   { "invert", GL_INVERT } } /// Bitwise inverts the current stencil value.

/// z_buf_operations
//{ { "always", GL_ALWAYS },
//  { "never", GL_NEVER },
//  { "less", GL_LESS },
//  { "equal", GL_GEQUAL },
//  { "lequal", GL_LEQUAL },
//  { "greater", GL_GREATER },
//  { "notequal", GL_NOTEQUAL },
//  { "gequal", GL_GEQUAL } }
