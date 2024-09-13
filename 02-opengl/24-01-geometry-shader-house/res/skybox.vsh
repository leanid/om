#version 320 es
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

uniform mat4 view;
uniform mat4 projection;

out vec3 v_tex_coords;

void main()
{
    vec4 pos = projection * view * vec4(a_position, 1.0);
    gl_Position = pos.xyww;
    // interpolated cube vertex position match cubemap texture coordinates
    v_tex_coords = a_position;
}
