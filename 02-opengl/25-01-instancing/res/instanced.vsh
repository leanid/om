#version 320 es
layout (location = 0) in vec3 a_position;
layout (location = 2) in vec2 a_uv;

out VS_OUT {
    vec2 uv;
} vs_out;

uniform vec2 offsets[100];

void main()
{
    vec2 offset = offsets[gl_InstanceID];
    vs_out.uv     = a_uv;
    gl_Position = vec4(a_position.x + offset.x, a_position.y + offset.y, a_position.z, 1.0);
}
