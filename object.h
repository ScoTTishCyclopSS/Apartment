#pragma once
#include "render_stuff.h"
#include <iostream>

using namespace glm;

class Mesh {
public:
	GLuint vbo;
	GLuint ebo;
	GLuint vao;
	GLuint texture;
	GLuint normal;
};

class Object {
public:
	std::string name;
	Mesh mesh;

	int countAttribsPerVertex;
	int countVertices;
	int countTriangles;
	const float* vertices;
	const unsigned* triangles;
	int frames;

	vec3 position;
	vec3 scale;

	bool isLightsource;
	bool isAnimated;

	// methods 

	Object(std::string name_) { // .obj
		name = name_;
		position = vec3(0.0f);
		scale = vec3(1.0f);
		mesh.texture = 0;
		mesh.normal = 0;
		isLightsource = false;
		isAnimated = false;
	};

	Object(
		std::string name_,
		const int attPerV,
		const int v_num,
		const int t_num,
		const float* v,
		const unsigned* t
	) { // .cpp
		position = vec3(0.0f);
		scale = vec3(1.0f);
		countAttribsPerVertex = attPerV;
		countVertices = v_num;
		countTriangles = t_num;
		vertices = v;
		triangles = t;
		name = name_;
		isLightsource = false;
		isAnimated = false;
	};

	void initModel(Shader shader); // .cpp
	void initAnimModel(Shader shader); // .cpp

	void drawObject(
		Shader shader, 
		const glm::mat4& viewMat,
		const glm::mat4& projectMat, 
		const glm::mat4& modelMat
	);

	void drawAnimObject(
		Shader shader,
		const glm::mat4& viewMat,
		const glm::mat4& projectMat,
		const glm::mat4& modelMat
	);

	bool loadMesh( // .obj
		const std::string& fileName, 
		Shader shader
	);
};
