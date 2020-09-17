#version 320 es
precision mediump float;

out vec4 frag_color;

in VS_OUT {
    vec2 uv;
} vs_out;


struct rock_material
{
    sampler2D tex_diffuse0;
    sampler2D tex_specular0;
};

uniform rock_material material;

void main()
{
    frag_color = texture(material.tex_diffuse0, vs_out.uv);
    frag_color += vec4(0.01, 0, 0, 0) * texture(material.tex_specular0, vs_out.uv);
    // frag_color += vec4(0, 1, 0, 0);
}
