#version 330

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 viewMat;       
uniform mat4 projMat;       
uniform mat4 modelMat;

out vec2 texCoord;
out vec4 clipSpace;

void main()
{
    gl_Position = projMat * viewMat * modelMat * vec4(aPos, 1.0);
    texCoord = vec2(aTexCoord.x / 2.0 + 0.5, aTexCoord.y / 2.0 + 0.5) * 6.0;
    clipSpace = gl_Position;
}