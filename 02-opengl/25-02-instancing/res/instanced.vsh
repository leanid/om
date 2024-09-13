#version 320 es
layout (location = 0) in vec3 a_position;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec2 a_offset;

out VS_OUT {
    vec2 uv;
} vs_out;

void main()
{
    vs_out.uv     = a_uv;

    if (gl_InstanceID % 2 == 0)
    {
        gl_Position = vec4(2.0 * a_position.x + a_offset.x, 2.0 * a_position.y + a_offset.y, a_position.z, 1.0);
    } else
    {
        gl_Position = vec4(a_position.x + a_offset.x, a_position.y + a_offset.y, a_position.z, 1.0);
    }
}
