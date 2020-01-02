#version 300 es
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 v_normal;
out vec3 v_frag_pos;
out vec2 v_tex_coords;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    v_frag_pos = vec3(model * vec4(a_pos, 1.0));
    //Normal      = aNormal; // forgot transform?
    mat4 inverse_model = inverse(model);
    v_normal = mat3(transpose(inverse_model)) * a_normal;
    v_tex_coords = a_tex_coords;
}
