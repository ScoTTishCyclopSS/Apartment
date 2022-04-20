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

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 color;

out vec3 out_color;
out vec2 out_TexCoord;

void main()
{
    gl_Position = PVM * vec4(position, 1.0);
    //out_color = color;
    out_TexCoord = vec2(texCoord);
}