#version 320 es
precision mediump float;

out vec4 frag_color;

in vec2 tex_coords;


struct nanosuit_material
{
    sampler2D tex_diffuse0;
    sampler2D tex_specular0;
};

uniform nanosuit_material material;

void main()
{
    frag_color = texture(material.tex_diffuse0, tex_coords);
    frag_color += texture(material.tex_specular0, tex_coords);
    frag_color += vec4(0, 1, 0, 0);
}
