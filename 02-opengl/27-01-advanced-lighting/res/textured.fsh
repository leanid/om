#version 320 es
precision mediump float;

out vec4 frag_color;

in VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 uv;
} vs_out;


struct nanosuit_material
{
    sampler2D tex_diffuse0;
    sampler2D tex_specular0;
};

uniform nanosuit_material material;

uniform vec3 light_pos;
uniform vec3 view_pos;
uniform bool blinn;

void main()
{
    vec3 color = texture(material.tex_diffuse0, vs_out.uv).rgb;
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 light_dir = normalize(light_pos - vs_out.pos);
    vec3 normal = normalize(vs_out.normal);
    float diff = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 view_dir = normalize(view_pos - vs_out.pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfway_dir = normalize(light_dir + view_dir);
        spec = pow(max(dot(normal, halfway_dir), 0.0), 32.0);
    }
    else
    {
        vec3 reflect_dir = reflect(-light_dir, normal);
        spec = pow(max(dot(view_dir, reflect_dir), 0.0), 8.0);
    }
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    frag_color = vec4(ambient + diffuse + specular, 1.0);
}
