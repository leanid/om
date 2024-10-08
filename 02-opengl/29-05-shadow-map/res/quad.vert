#version 320 es
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out VS_OUT {
    vec3 normal;
    vec2 uv;
} vs_out;

void main()
{
    vs_out.normal = a_normal;
    vs_out.uv     = a_uv;
    gl_Position = vec4(a_position.x, a_position.y, a_position.z, 1.0);
}
