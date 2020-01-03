#version 300 es
precision mediump float;

out vec4 frag_color;

in vec2 v_tex_coords;

struct default_mat
{
    sampler2D tex_diffuse0;
};

uniform default_mat material;

void main()
{
    frag_color = texture2D(material.tex_diffuse0, v_tex_coords);
}
