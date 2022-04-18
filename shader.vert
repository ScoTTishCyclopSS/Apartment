#version 330

layout(location = 0) in vec3 position;
out vec4 color;
uniform mat4  PVM;   // transformation matrix

void main()
{
    gl_Position = PVM * vec4(position, 1.0);
    color.a = 1.0;
    color.rgb = position * 0.5 + 0.5;
}
