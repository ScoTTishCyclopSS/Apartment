#version 330

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection
  bool  isLight;
};

uniform Material material;
uniform sampler2D textureSampler;

in vec3 position_v;
in vec2 texCoord_v;
in vec3 normal_v;
in vec3 LightingColor; 

out vec4 fragmentColor;

void main()
{
    fragmentColor =  texture(textureSampler, texCoord_v) * vec4(LightingColor, 1.0f);
}
