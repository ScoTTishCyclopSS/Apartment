#version 330

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection
  bool  useTexture;         // defines whether the texture is used or not
};

uniform Material material;
uniform sampler2D textureSampler;
smooth in vec4 color_v;
smooth in vec2 texCoord;
out vec4 fragmentColor;

void main()
{
    if(material.useTexture)
        fragmentColor = color_v * texture(textureSampler, texCoord);
}
