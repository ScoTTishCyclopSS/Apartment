#pragma once
#include "render_stuff.h"

using namespace glm;

class Mesh {
public:
	GLuint vbo;
	GLuint ebo;
	GLuint vao;

	// material
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shiness;
	GLuint texture;
};

class Object {
public:
	Mesh mesh;
	vec3 position;

	int countAttribsPerVertex;
	int countVertices;
	int countTriangles;
	const float* vertices;
	const unsigned* triangles;

	Object(const int attPerV, const int v_num, const int t_num, const float* v, const unsigned* t) {
		position = vec3(0.0f, 0.0f, 0.0f);
		countAttribsPerVertex = attPerV;
		countVertices = t_num;
		countTriangles = v_num;
		vertices = v;
		triangles = t;
	};

	void initModel(Shader shader);

	void drawObject(Shader shader, const glm::mat4& viewMat, const glm::mat4& projectMat, const glm::mat4& modelMat);

};
