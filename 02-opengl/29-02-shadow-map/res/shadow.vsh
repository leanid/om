#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 uv;
    vec4 frag_pos_light_space;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 light_space_matrix;

void main()
{
    vs_out.frag_pos = vec3(model * vec4(a_position, 1.0));
    vs_out.normal = transpose(inverse(mat3(model))) * a_normal;
    vs_out.uv = a_uv;
    vs_out.frag_pos_light_space = light_space_matrix * vec4(vs_out.frag_pos, 1.0);
    gl_Position = projection * view * vec4(vs_out.frag_pos, 1.0);
}
