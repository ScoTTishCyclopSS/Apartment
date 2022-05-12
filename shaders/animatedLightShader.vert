#version 430 core

uniform mat4 viewMat;       
uniform mat4 projMat;       
uniform mat4 modelMat;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 aBoneIds; 
layout (location = 6) in vec4 aWeights;
	
out vec3 position_v;
out vec2 texCoord_v;
out vec3 normal_v;
out mat3 TBN;

int maxBones = 50;
uniform int maxBonesInfluence = 4;
uniform mat4 finalBonesMatrices[50];

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < maxBonesInfluence; i++)
    {
        if(aBoneIds[i] != -1)
        {
            if(aBoneIds[i] >= maxBones) 
            {
                totalPosition = vec4(aPos, 1.0f);
                break;
            }
            vec4 localPosition = finalBonesMatrices[aBoneIds[i]] * vec4(aPos, 1.0f);
            totalPosition += localPosition * aWeights[i];
            vec3 localNormal = mat3(finalBonesMatrices[aBoneIds[i]]) * aNorm;
        }
    }

    vec3 T = normalize(vec3(modelMat * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(modelMat * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(modelMat * vec4(aNorm, 0.0)));
    TBN = mat3(T, B, N);
		
    gl_Position =  projMat * viewMat * modelMat * totalPosition;
    normal_v = mat3(modelMat) * aNorm;
    texCoord_v = aTexCoord;
    position_v = vec3(modelMat * vec4(aPos, 1.0));
}