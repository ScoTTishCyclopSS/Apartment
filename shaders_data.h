#ifndef __SHADERS_DATA_H
#define __SHADERS_DATA_H

#include <iostream>

const std::string vertexShader(
	"#version 140\n"
	"in vec3 position;\n"
	"out vec3 color;\n"
	"uniform mat4 PVM;\n"
	"void main() {\n"
	"	gl_Position = PVM * vec4(position, 1.0f);\n"
	"}\n"
);

const std::string fragmentShader(
	"#version 140\n"
	"in vec4 color;\n"
	"out vec4 fragmentColor;\n"
	"uniform mat4 PVM;\n"
	"void main() {\n"
	"	fragmentColor = color;\n"
	"}\n"
);

#endif