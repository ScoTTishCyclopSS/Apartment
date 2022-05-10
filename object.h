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
	std::vector<vec3> tangents;
};

class Material {
public:
	// material
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shiness;
};

class Object {
public:
	std::string name;
	bool isLightsource;
	Mesh mesh;
	Material material;

	int countAttribsPerVertex;
	int countVertices;
	int countTriangles;
	const float* vertices;
	const unsigned* triangles;

	vec3 position;
	vec3 scale;

	// methods 

	Object(std::string name_) {
		name = name_;
		position = vec3(0.0f);
		scale = vec3(1.0f);
		mesh.texture = 0;
		mesh.normal = 0;
	};

	Object(std::string name_, const int attPerV, const int v_num, const int t_num, const float* v, const unsigned* t) {
		position = vec3(0.0f);
		scale = vec3(1.0f);
		countAttribsPerVertex = attPerV;
		countVertices = v_num;
		countTriangles = t_num;
		vertices = v;
		triangles = t;
		name = name_;
		isLightsource = false;
	};

	void initModel(Shader shader);

	void drawObject(Shader shader, const glm::mat4& viewMat, const glm::mat4& projectMat, const glm::mat4& modelMat);

	bool loadMesh(const std::string& fileName, Shader shader);
};
