#version 300 es
precision mediump float;

struct Material
{
    vec3      ambient;
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

uniform Material material;

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

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

//uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

vec3 calc_dir_light(vec3 normal, vec3 viewDir, vec3 diffuse_color,
                    vec3 specular_color)
{
    vec3 lightDir = normalize(-dirLight.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3  reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = dirLight.ambient * diffuse_color;
    vec3 diffuse  = dirLight.diffuse * diff * diffuse_color;
    vec3 specular = dirLight.specular * spec * specular_color;
    return (ambient + diffuse + specular);
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,
                      vec3 diffuse_color, vec3 specular_color)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3  reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
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

vec3 calc_spot_light(SpotLight spot_light, vec3 Normal, vec3 FragPos, vec3 viewDir,
                     vec3 diffuse_color, vec3 specular_color)
{
    vec3 result = vec3(0.0, 0.0, 0.0);
    // ambient
    vec3 ambient = spot_light.ambient * diffuse_color;

    // calculate direction from light position
    vec3  lightDir = normalize(spot_light.position - FragPos);
    float theta    = dot(lightDir, normalize(-spot_light.direction));

    if (theta > spot_light.outer_cut_off)
    {
        // do lighting calculations
        // diffuse
        vec3  norm    = normalize(Normal);
        float diff    = max(dot(norm, lightDir), 0.0);
        vec3  diffuse = spot_light.diffuse * (diff * diffuse_color);

        // specular
        vec3  viewDir    = normalize(spot_light.position - FragPos); // viewDir == lightDir camera
        vec3  reflectDir = reflect(-lightDir, norm);
        float spec =
            pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = spot_light.specular * (spec * specular_color);

        float distance    = length(spot_light.position - FragPos);
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
    vec3 diffuse_color  = texture(material.diffuse, TexCoords).rgb;
    vec3 specular_color = texture(material.specular, TexCoords).rgb;

    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(spot_light.position - FragPos); // camera is spot_light

    // phase 1: Directional lighting
    vec3 result = calc_dir_light(norm, viewDir, diffuse_color, specular_color);
    // phase 2: Point lights
    for (int i = 0; i < 4; i++)
    {
        result += calc_point_light(pointLights[i], norm, FragPos, viewDir,
                                   diffuse_color, specular_color);
    }
    // phase 3: Spot light
    result += calc_spot_light(spot_light, norm, FragPos, viewDir,
                              diffuse_color, specular_color);

    FragColor = vec4(result, 1.0);
}
