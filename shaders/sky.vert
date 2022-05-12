#version 330
 

layout (location = 0) in vec3 position_v;
uniform mat4 viewMat;       
uniform mat4 projMat;       

out vec3 skyCoords;

void main()
{
	gl_Position = projMat * viewMat * vec4(position_v, 1.0);
	skyCoords = position_v;
}