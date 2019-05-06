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
float       angle         = 45.f;
glm::vec3   rotate_axis   = { 1.f, 0.f, 0.f };
glm::vec3   move_camera   = { 0.f, 0.f, -1.f + -3.f };

// mini grammatics for parser
//
// float     z_near           =       3.f;
// glm::vec3 move_camera      =       { 0.f, 0.f, -2.f };
//
// <type>   <identifier>  <operation> <expression>;
// type:   <float, std::string, glm::vec3>
// identifier: <a-zA-Z_0-9>
// operation: <+, -, =, /, *>
// expression: <float_literal,
//              string_literal,
//              identifier,
//              *_literal operation expression,
//              identifier operation expression,
//              '{' expression ',' expression ',' expression '}'
