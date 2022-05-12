#version 330

struct PointLight {
    vec3 diffuse;
    vec3 ambient;            
    vec3 specular;          

    float constant;
    float linear;
    float quadratic; 

    bool blinn;
    vec3 position;
};

struct DirectLight {
    vec3 diffuse;
    vec3 ambient;            
    vec3 specular;

    vec3 direction;
};

struct SpotLight {
    vec3 diffuse;
    vec3 ambient;            
    vec3 specular;

    vec3 direction;
};

#define LIGHTS_COUNT 3

uniform PointLight pointLights [LIGHTS_COUNT];
uniform DirectLight directLight;
uniform SpotLight spotLight;

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;
uniform sampler2D roughSampler;
uniform vec3 viewPos;

in mat3 TBN;
in vec3 position_v;
in vec2 texCoord_v;
in vec3 normal_v;

out vec4 fragmentColor;

vec3 AddPointLight(PointLight plight, vec3 normal, vec3 viewDir);

void main()
{
    vec3 viewDir = normalize(viewPos - position_v);
    vec3 normal = texture2D(normalSampler, texCoord_v).xyz;
    vec3 result = vec3(0.0);

    normal = normal * 2.0 - 1.0;
    //normal = normalize(TBN * normal);
    normal = normalize(normal_v);

    for (int i = 0; i < LIGHTS_COUNT; i++) {
       result += AddPointLight(pointLights[i], normal, viewDir);
    }

    fragmentColor = vec4(result, 1.0f);
}

vec3 AddPointLight(PointLight plight, vec3 normal, vec3 viewDir)
{
    // calculate directions
    vec3 lightDir = normalize(plight.position - position_v);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // setup the light
    float diff = max(dot(lightDir, normal), 0.0);
    float spec = 0.0;
    if (plight.blinn)
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);
    else
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

    float dist = length(plight.position - position_v);
    // attenuation - loss of light intensity over distance
    float attenuation = 1.0 / (plight.constant + plight.linear * dist + plight.quadratic * (dist * dist));
    
    // final properties
    vec3 ambient  = plight.ambient  * vec3(0.01); // texture(textureSampler, texCoord_v)
    vec3 diffuse  = plight.diffuse  * diff * vec3(texture(textureSampler, texCoord_v));
    vec3 specular = plight.specular * spec * vec3(0.1); // texture(roughSampler, texCoord_v)

    // combine light
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}
