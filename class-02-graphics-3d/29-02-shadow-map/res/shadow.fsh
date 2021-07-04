#version 330 core
out vec4 frag_color;

in VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 uv;
    vec4 frag_pos_light_space;
} fs_in;

struct mesh_material
{
    sampler2D tex_diffuse0;
    sampler2D tex_specular0;
};

uniform mesh_material material;

uniform sampler2D tex_shadow_map;

uniform vec3 light_pos;
uniform vec3 view_pos;

float ShadowCalculation(vec4 frag_pos_light_space)
{
    // perform perspective divide
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;

    // Because the depth from the depth map is in the range [0,1]
    // and we also want to use projCoords to sample from the depth map,
    // we transform the NDC coordinates to the range [0,1]:
    proj_coords = proj_coords * 0.5 + 0.5;

    // closest depth from light point of view
    float closest_depth = texture(tex_shadow_map, proj_coords.xy).r;

    float current_depth = proj_coords.z;

    float shadow = current_depth > closest_depth  ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    vec3 color = texture(material.tex_diffuse0, fs_in.uv).rgb;
    vec3 normal = normalize(fs_in.normal);
    vec3 light_color = vec3(1.0);
    // ambient
    vec3 ambient = 0.15 * color;
    // diffuse
    vec3 light_dir = normalize(light_pos - fs_in.frag_pos);
    float diff = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diff * light_color;
    // specular
    vec3 view_dir = normalize(view_pos - fs_in.frag_pos);
    float spec = 0.0;
    vec3 halfway_dir = normalize(light_dir + view_dir);
    spec = pow(max(dot(normal, halfway_dir), 0.0), 64.0);
    vec3 specular = spec * light_color;
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.frag_pos_light_space);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    frag_color = vec4(lighting, 1.0);
}
