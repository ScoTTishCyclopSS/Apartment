#version 330

struct Material {      // structure that describes currently used material
  vec3  ambient;       // ambient component
  vec3  diffuse;       // diffuse component
  vec3  specular;      // specular component
  float shininess;     // sharpness of specular reflection
  bool  useTexture;    // defines whether the texture is used or not
};

uniform mat4 PVM;   
uniform mat4 viewMat;       
uniform mat4 modelMat;       
uniform mat4 normMat;  

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec2 out_TexCoord;

void main()
{
    gl_Position = PVM * vec4(position, 1);
    out_TexCoord = vec2(texCoord);
}