#version 330

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection
};

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;
uniform vec3 lightPos; // by user
uniform vec3 viewPos; // by user

in mat3 TBN;
in vec3 position_v;
in vec2 texCoord_v;
in vec3 normal_v;

out vec4 fragmentColor;

void main()
{
    float ambientStrength = 0.5;
    float specularStrength = 0.8;
    vec3 lightColor = vec3(0.9);
    float shininess_ = 32.0;

    vec3 norm = texture2D(normalSampler, texCoord_v).xyz;
    norm = norm * 2.0 - 1.0;
    norm = normalize(/*TBN * norm*/normal_v);

    vec3 lightDir = /*TBN */ normalize(lightPos - position_v);
    vec3 diffuse_ = max(dot(lightDir, norm), 0.0) * lightColor;
    vec3 ambient_ = lightColor * ambientStrength;
    vec3 viewDir = /*TBN */ normalize(viewPos - position_v);

    //if !blinn
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess_);

    //if blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess_);

    vec3 specular_ = specularStrength * spec * lightColor;

    fragmentColor =  vec4((ambient_ + diffuse_ + specular_), 1.0f) * texture(textureSampler, texCoord_v);
}
