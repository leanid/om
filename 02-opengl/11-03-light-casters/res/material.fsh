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

struct Light
{
    vec3 position;
    vec3 direction;

    float cut_off; // cos from light_dir to spot Phi
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

void main()
{
    vec3 diffuse_color  = texture(material.diffuse, TexCoords).rgb;
    vec3 specular_color = texture(material.specular, TexCoords).rgb;

    // ambient
    vec3 ambient = light.ambient * diffuse_color;

    // calculate direction from light position
    vec3  lightDir = normalize(light.position - FragPos);
    float theta    = dot(lightDir, normalize(-light.direction));

    if (theta > light.outer_cut_off)
    {
        // do lighting calculations
        // diffuse
        vec3  norm    = normalize(Normal);
        float diff    = max(dot(norm, lightDir), 0.0);
        vec3  diffuse = light.diffuse * (diff * diffuse_color);

        // specular
        vec3  viewDir    = normalize(viewPos - FragPos);
        vec3  reflectDir = reflect(-lightDir, norm);
        float spec =
            pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * specular_color);

        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance +
                                   light.quadratic * (distance * distance));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        // do not forget it is cosinus positive value
        float epsilon   = light.cut_off - light.outer_cut_off;
        float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

        diffuse *= intensity;
        specular *= intensity;

        vec3 result = (ambient + diffuse + specular); // * objectColor;
        FragColor   = vec4(result, 1.0);
    }
    else
    { // else, use ambient light so scene isn't completely dark outside the
      // spotlight.
        FragColor = vec4(ambient, 1.0);
    }
}
