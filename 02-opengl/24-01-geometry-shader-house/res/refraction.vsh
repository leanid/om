#version 320 es
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

out vec3 v_normal;
out vec3 v_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // we need only direction without translation
    v_normal = mat3(transpose(inverse(model))) * a_normal;
    v_position = vec3(model * vec4(a_position, 1.0));
    gl_Position = projection * view * model * vec4(a_position, 1.0);
}
