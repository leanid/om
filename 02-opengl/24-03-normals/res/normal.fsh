#version 320 es
precision mediump float;

out vec4 frag_color;

struct nanosuit_material
{
    sampler2D tex_diffuse0;
    sampler2D tex_specular0;
};

uniform nanosuit_material material;

in vec2 tex_uv;

void main()
{
    // just old same code for render model
    frag_color = texture(material.tex_diffuse0, tex_uv);
    frag_color += texture(material.tex_specular0, tex_uv);
    // only use it
    frag_color += vec4(0.0, 1.0, 0.0, 1.0);
}
