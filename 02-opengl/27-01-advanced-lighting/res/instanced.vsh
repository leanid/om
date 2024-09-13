#version 320 es
layout (location = 0) in vec3 a_position;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in mat4 a_instance_matrix;

out VS_OUT {
    vec2 uv;
} vs_out;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vs_out.uv     = a_uv;
    gl_Position = projection * view * a_instance_matrix * vec4(a_position.x, a_position.y, a_position.z, 1.0);
}
