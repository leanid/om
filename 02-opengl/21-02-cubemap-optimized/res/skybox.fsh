#version 300 es
precision mediump float;
out vec4 frag_color;

in vec3 v_tex_coords;

struct default_mat
{
    samplerCube tex_cubemap0;
};

uniform default_mat material;

void main()
{
    frag_color = textureCube(material.tex_cubemap0, v_tex_coords);
}
