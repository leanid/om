#version 320 es
precision mediump float;

out vec4 frag_color;

in VS_OUT {
    vec3 normal;
    vec2 uv;
} vs_out;

struct mesh_material
{
    sampler2D tex_diffuse0;
    sampler2D tex_specular0;
};

uniform mesh_material material;

void main()
{

    vec4 col = texture(material.tex_diffuse0, vs_out.uv);
    float grayscale = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
    frag_color = vec4(vec3(grayscale), 1.0);
}
