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

glm::vec3 cam_pos = { 0.f, 0.f, 3.f };
glm::vec3 cam_dir = { 0.f, 0.f, -1.f };
// to understand normal transform - try rotate cube
float     angle       = 0.000f;
glm::vec3 rotate_axis = { 0.f, 1.f, 0.f };

glm::vec3 clear_color = { 0.0f, 0.0f, 0.0f };

std::string z_buf_operation = "notequal";

//{ { "always", GL_ALWAYS },
//  { "never", GL_NEVER },
//  { "less", GL_LESS },
//  { "equal", GL_GEQUAL },
//  { "lequal", GL_LEQUAL },
//  { "greater", GL_GREATER },
//  { "notequal", GL_NOTEQUAL },
//  { "gequal", GL_GEQUAL } }
