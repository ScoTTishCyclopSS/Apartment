#version 330

uniform sampler2D textureSampler;
in vec2 texCoord;
out vec4 fragmentColor;

void main()
{
	fragmentColor = vec4(vec3(1.0), 1.0); //texture(textureSampler, texCoord);
}
