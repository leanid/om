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

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
    vec3 diffuse_color = texture(material.diffuse, TexCoords).rgb;
    vec3 specular_color = texture(material.specular, TexCoords).rgb;
    // ambient
    vec3 ambient = light.ambient * diffuse_color;

    // diffuse
    vec3  norm     = normalize(Normal);
    vec3  lightDir = normalize(light.position - FragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = light.diffuse * (diff * diffuse_color);

    // specular
    vec3  viewDir    = normalize(viewPos - FragPos);
    vec3  reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3  specular = light.specular * (spec * specular_color);

    vec3 result = (ambient + diffuse + specular); // * objectColor;
    FragColor   = vec4(result, 1.0);
}
