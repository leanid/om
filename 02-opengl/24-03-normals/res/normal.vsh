#version 320 es
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out VS_OUT {
    vec3 normal;
    vec2 uv;
} vs_out;

uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = view * model * vec4(a_position, 1.0);
    // can do it only, but skip optimization
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * a_normal, 0.0)));
    vs_out.uv = a_uv;
}
