#version 330

in vec3 skyCoords;
uniform samplerCube textureSampler;
out vec4 fragmentColor;

void main()
{
	fragmentColor = texture(textureSampler, skyCoords);
}
