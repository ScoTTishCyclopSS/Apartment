#version 330

uniform sampler2D textureSampler;
in vec2 texCoord;
out vec4 fragmentColor;

void main()
{
	fragmentColor = texture(textureSampler, texCoord);
}
