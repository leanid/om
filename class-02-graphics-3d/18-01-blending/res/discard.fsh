#version 300 es
precision mediump float;

in vec2 v_tex_coords;

struct default_mat
{
    sampler2D tex_diffuse0;
};

uniform default_mat material;

out vec4 frag_color;

void main()
{
    vec4 color = texture2D(material.tex_diffuse0, v_tex_coords);
    if (color.a < 0.1)
    {
    //    discard;
    }
    frag_color = color;
}

