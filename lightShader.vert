#version 330

uniform mat4 PVM;   
uniform mat4 viewMat;       
uniform mat4 projMat;       
uniform mat4 modelMat;       
uniform mat4 normMat;  

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 texCoord_v;
out vec3 normal_v;
out vec3 position_v;

out vec3 LightingColor;

void main()
{
    gl_Position = PVM * vec4(position, 1.0);
    
    // shading
    vec3 Position = vec3(modelMat * vec4(position, 1.0));
    vec3 Normal = mat3(transpose(inverse(modelMat))) * normal;
    
    // ambient
    float ambientStrength = 0.3;
    vec3 lightColor = vec3(1.0f);
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 positionLight = vec3(10.0f, 0.0f, 0.0f);
    vec3 lightDir = normalize(positionLight - Position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 4.0; // this is set higher to better show the effect of Gouraud shading 
    vec3 viewPos = vec3(1.0);
    vec3 viewDir = normalize(viewPos - Position);

    // if (!bliss)
    //vec3 reflectDir = reflect(-lightDir, norm);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    //vec3 specular = specularStrength * spec * lightColor;  

    // bliss
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 1.0); // 0.1 = shininess
    vec3 specular = lightColor * spec;

    LightingColor = ambient + diffuse + specular;

    texCoord_v = texCoord;
}