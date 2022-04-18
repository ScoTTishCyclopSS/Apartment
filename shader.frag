#version 330

out vec4 fragmentColor;
in vec4 color;
uniform mat4  PVM;   // transformation matrix

void main()
{
    fragmentColor = color;
}
