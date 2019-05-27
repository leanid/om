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

struct Light
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

uniform Light light;

// uniform vec3 objectColor;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

vec3 calc_dir_light(vec3 normal, vec3 viewDir, vec3 diffuse_color,
                    vec3 specular_color)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3  reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient * diffuse_color;
    vec3 diffuse  = light.diffuse * diff * diffuse_color;
    vec3 specular = light.specular * spec * specular_color;
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

void main()
{
    // properties
    vec3 diffuse_color  = texture(material.diffuse, TexCoords).rgb;
    vec3 specular_color = texture(material.specular, TexCoords).rgb;

    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = calc_dir_light(norm, viewDir, diffuse_color, specular_color);
    // phase 2: Point lights
    for (int i = 0; i < 4; i++)
    {
        result += calc_point_light(pointLights[i], norm, FragPos, viewDir,
                                   diffuse_color, specular_color);
    }
    // phase 3: Spot light
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}
