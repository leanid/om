#include <string>

// you can use this header file directly in your build
// also you can update during runtime this values with
// properties_reader.hxx

float       z_near        = 3.f;
float       z_far         = 100.f;
std::string title         = "1-triangles, 2-lines, 3-line-strip, 4-line-loop";
float       fovy          = 45.f;
float       screen_width  = 640.f;
float       screen_height = 480.f;
float       aspect        = screen_height / screen_width;
