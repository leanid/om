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
    vec3 col = texture2D(material.tex_diffuse0, v_tex_coords).rgb;
    float average = (col.r + col.g + col.b) / 3.0;
    frag_color = vec4(average, average, average, 1.0);
}
