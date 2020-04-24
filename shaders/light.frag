#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

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

#define NUM_POINTLIGHTS 10
#define NUM_SPOTLIGHTS 10

uniform vec3 viewPos;

uniform DirectionalLight dLight_;
uniform PointLight pLights_[NUM_POINTLIGHTS];
uniform SpotLight sLights_[NUM_SPOTLIGHTS];

uniform Material material;

vec3 calculateAmbientLight(BaseLight baseLight) {
    return baseLight.ambient * texture(material.diffuse, TexCoords).rgb;
}

vec3 calculateDiffuseLight(BaseLight baseLight, vec3 lightDirection, vec3 normal) {
    float diff = max(dot(normal, lightDirection), 0.0f);
    return baseLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
}

vec3 calculateSpecularLight(BaseLight baseLight, vec3 lightDirection, vec3 viewDirection, vec3 normal) {
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0f), material.shininess);
    return baseLight.specular * spec * texture(material.specular, TexCoords).rgb;
}

vec3 calculateDirectionalLight(DirectionalLight dLight, vec3 viewDirection, vec3 normal) {
    vec3 lightDirection = normalize(-dLight.direction);

    vec3 ambient = calculateAmbientLight(dLight.base);
    vec3 diffuse = calculateDiffuseLight(dLight.base, lightDirection, normal);
    vec3 specular = calculateSpecularLight(dLight.base, lightDirection, viewDirection, normal);

    return (ambient + diffuse + specular);
}

float calculateAttenuation(Attenuation attenuation, float distance) {
    float factor = attenuation.constant + attenuation.linear * distance + attenuation.quadratic * (distance * distance);
    if (factor == 0.0f)
        factor = 1.0f;
    return 1.0f / factor;
}

vec3 calculatePointLight(PointLight pLight, vec3 viewDirection, vec3 normal) {
    vec3 fragToLightVector = pLight.position - FragPos;
    vec3 lightDirection = normalize(fragToLightVector);

    vec3 ambient = calculateAmbientLight(pLight.base);
    vec3 diffuse = calculateDiffuseLight(pLight.base, lightDirection, normal);
    vec3 specular = calculateSpecularLight(pLight.base, lightDirection, viewDirection, normal);

    float distance = length(fragToLightVector);
    float attenuation = calculateAttenuation(pLight.attenuation, distance);

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calculateSpotLight(SpotLight sLight, vec3 viewDirection, vec3 normal) {

    return vec3(0, 0, 0);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDirection = normalize(viewPos - FragPos);

    vec3 result = calculateDirectionalLight(dLight_, viewDirection, normal);
    //vec3 result;

    for (int i = 0; i < NUM_POINTLIGHTS; ++i){
        result += calculatePointLight(pLights_[i], viewDirection, normal);
    }

    for (int i = 0; i < NUM_SPOTLIGHTS; ++i){
        result += calculateSpotLight(sLights_[i], viewDirection, normal);
    }

    // emission
    vec3 emission = texture(material.emission, TexCoords).rgb;

    //vec3 result = ambient + diffuse + specular;// + emission;

    FragColor = vec4(result, 1.0f);
}