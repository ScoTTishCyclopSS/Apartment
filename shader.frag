#version 330

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection
  bool  useTexture;         // defines whether the texture is used or not
};

uniform mat4  PVM;   // transformation matrix
uniform sampler2D ourTexture;

in vec3 ourColor;
in vec2 texCoord;

out vec4 fragmentColor;

void main()
{
    fragmentColor = texture(ourTexture, texCoord);
}
