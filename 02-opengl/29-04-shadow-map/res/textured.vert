// #version 320 es // works on Linux and Windows
#version 330      // works on MacOSX
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 uv;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vs_out.pos    = a_position;
    vs_out.normal = a_normal;
    vs_out.uv     = a_uv;
    gl_Position = projection * view * model * vec4(a_position.x, a_position.y, a_position.z, 1.0);
}
