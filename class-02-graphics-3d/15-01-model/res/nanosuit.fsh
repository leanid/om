#version 300 es
precision mediump float;

struct nanosuit_material
{
    vec3      ambient;
    sampler2D tex_diffuse0;
    sampler2D tex_specular0;
    float     shininess;
};

uniform nanosuit_material material;

struct dir_light
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform dir_light direction_light;

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform PointLight pointLights[4];

struct SpotLight
{
    vec3 position;
    vec3 direction;

    float cut_off;       // cos from light_dir to spot Phi
    float outer_cut_off; // cos from light_dir to

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform SpotLight spot_light;

in vec3 v_frag_pos;
in vec3 v_normal;
in vec2 v_tex_coords;

out vec4 out_frag_color;

vec3 calc_dir_light(in vec3 normal, in vec3 view_dir, in vec3 diffuse_color,
                    in vec3 specular_color, in float shininess)
{
    vec3 light_dir = normalize(-direction_light.direction);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3  reflectDir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient  = direction_light.ambient * diffuse_color;
    vec3 diffuse  = direction_light.diffuse * diff * diffuse_color;
    vec3 specular = direction_light.specular * spec * specular_color;
    return (ambient + diffuse + specular);
}

vec3 calc_point_light(in PointLight light, in vec3 normal, in vec3 frag_pos,
                      in vec3 viewDir, in vec3 diffuse_color,
                      in vec3 specular_color, in float shininess)
{
    vec3 lightDir = normalize(light.position - frag_pos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3  reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance    = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient * diffuse_color;
    vec3 diffuse  = light.diffuse * diff * diffuse_color;
    vec3 specular = light.specular * spec * specular_color;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 calc_spot_light(in SpotLight spot_light, in vec3 normal, in vec3 frag_pos,
                     in vec3 viewDir, in vec3 diffuse_color,
                     in vec3 specular_color, in float shininess)
{
    vec3 result = vec3(0.0, 0.0, 0.0);
    // ambient
    vec3 ambient = spot_light.ambient * diffuse_color;

    // calculate direction from light position
    vec3  lightDir = normalize(spot_light.position - frag_pos);
    float theta    = dot(lightDir, normalize(-spot_light.direction));

    if (theta > spot_light.outer_cut_off)
    {
        // do lighting calculations
        // diffuse
        vec3  norm    = normalize(normal);
        float diff    = max(dot(norm, lightDir), 0.0);
        vec3  diffuse = spot_light.diffuse * (diff * diffuse_color);

        // specular
        vec3  viewDir    = normalize(spot_light.position - frag_pos); // viewDir == lightDir camera
        vec3  reflectDir = reflect(-lightDir, norm);
        float spec =
            pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = spot_light.specular * (spec * specular_color);

        float distance    = length(spot_light.position - frag_pos);
        float attenuation = 1.0 / (spot_light.constant + spot_light.linear * distance +
                                   spot_light.quadratic * (distance * distance));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        // do not forget it is cosinus positive value
        float epsilon   = spot_light.cut_off - spot_light.outer_cut_off;
        float intensity = clamp((theta - spot_light.outer_cut_off) / epsilon, 0.0, 1.0);

        diffuse *= intensity;
        specular *= intensity;

        result = (ambient + diffuse + specular);
    }
    else
    { // else, use ambient light so scene isn't completely dark outside the
      // spotlight.
        result = ambient;
    }
    return result;
}

void main()
{
    // properties
    vec3 ambient_color  = material.ambient;
    vec3 diffuse_color  = texture2D(material.tex_diffuse0, v_tex_coords).rgb;
    vec3 specular_color = texture2D(material.tex_specular0, v_tex_coords).rgb;
    float shininess     = material.shininess;

    vec3 v_normal = normalize(v_normal);
    vec3 view_dir = normalize(spot_light.position - v_frag_pos); // camera is spot_light

    vec3 result = ambient_color;

    // phase 1: Directional lighting
    result += calc_dir_light(v_normal, view_dir, diffuse_color, specular_color, shininess);

    // phase 2: Point lights
    for (int i = 0; i < 4; i++)
    {
        result += calc_point_light(pointLights[i], v_normal, v_frag_pos, view_dir,
                                   diffuse_color, specular_color, shininess);
    }

    // phase 3: Spot light
    result += calc_spot_light(spot_light, v_normal, v_frag_pos, view_dir,
                              diffuse_color, specular_color, shininess);

    out_frag_color = vec4(result, 1.0);
}
