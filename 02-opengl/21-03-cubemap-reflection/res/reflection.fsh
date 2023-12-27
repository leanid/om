#version 300 es
precision mediump float;
out vec4 frag_color;

in vec3 v_normal;
in vec3 v_position;

uniform vec3 camera_pos;
struct defalt_mat
{
    samplerCube tex_cubemap0;
};

uniform defalt_mat material;

void main()
{
    vec3 I = normalize(v_position - camera_pos);
    vec3 R = reflect(I, normalize(v_normal));
    frag_color = vec4(textureCube(material.tex_cubemap0, R).rgb, 1.0);
}
