#version 330

uniform mat4 viewMat;       
uniform mat4 projMat;       
uniform mat4 modelMat;
uniform float t;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec3 aNextPos;

out vec3 position_v;
out vec2 texCoord_v;
out vec3 normal_v;
out float t_v;
out mat3 TBN;

void main()
{
    vec3 T = normalize(vec3(modelMat * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(modelMat * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(modelMat * vec4(aNorm, 0.0)));
    TBN = mat3(T, B, N);

    gl_Position = projMat * viewMat * modelMat * vec4(aPos, 1.0);
    normal_v = mat3(modelMat) * aNorm;
    texCoord_v = aTexCoord;

    position_v = vec3(modelMat * vec4(aPos, 1.0)); // mix(aPos, aNextPos, t)

    t_v = t;
}