#version 300 es
precision mediump float;
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float shininess; // [2, 4, ... 256]

in vec3 FragPos;
in vec3 Normal;

void main()
{
    float ambientStrength = 0.1;
    vec3  ambient         = ambientStrength * lightColor;

    vec3  norm     = normalize(Normal);
    vec3  lightDir = normalize(lightPos - FragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = diff * lightColor;

    float specularStrength = 0.5;
    vec3  viewDir          = normalize(viewPos - FragPos);
    vec3  reflectDir       = reflect(-lightDir, norm);
    float dotReflView      = dot(viewDir, reflectDir);
    float spec             = pow(max(dotReflView, 0.0), shininess);
    vec3  specular         = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor   = vec4(result, 1.0);
}
