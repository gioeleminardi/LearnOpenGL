#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

struct BaseLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionalLight {
    BaseLight base;
    vec3 direction;
};

struct PointLight {
    BaseLight base;
    Attenuation attenuation;
    vec3 position;
};

struct SpotLight {
    BaseLight base;
    Attenuation attenuation;

    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform DirectionalLight dLight;
uniform PointLight pLight;
uniform SpotLight sLight;

uniform Material material;


void main()
{
    // ambient
    vec3 ambient = pLight.base.ambient * texture(material.diffuse, TexCoords).rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = pLight.base.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = pLight.base.specular * spec * texture(material.specular, TexCoords).rgb;

    // emission
    vec3 emission = texture(material.emission, TexCoords).rgb;

    vec3 result = ambient + diffuse + specular;// + emission;

    FragColor = vec4(result, 1.0f);
}