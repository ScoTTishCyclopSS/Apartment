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
	GLuint rough;
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

	bool isLightsource = false;
	bool isAnimated = false;
	bool isSkybox = false;
	bool isTransparent = false;
	bool isBillboard = false;
	bool isCombined = false;
	bool isGlass = false;

	// methods 

	Object(std::string name_) { // .obj
		name = name_;
		position = vec3(0.0f);
		scale = vec3(1.0f);
		mesh.texture = 0;
		mesh.normal = 0;
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
	};

	void initModel(Shader shader); // for .cpp

	void drawObject(
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

class MyCurve {
public:
	std::vector<vec3> controlPoints;

	MyCurve(std::vector<vec3> points) {
		controlPoints = points;
	};

	float currX(float time) {
		float x = pow(1 - time, 3) * controlPoints[0].x;
			  x += 3 * time * pow(1 - time, 2) * controlPoints[1].x;
			  x += 3 * pow(time, 2) * (1 - time) * controlPoints[2].x;
			  x += pow(time, 3) * controlPoints[3].x;
			  x /= 3;
		return x;
	};

	float currZ(float time) {
		float z = pow(1 - time, 3) * controlPoints[0].z;
			  z += 3 * time * pow(1 - time, 2) * controlPoints[1].z;
			  z += 3 * pow(time, 2) * (1 - time) * controlPoints[2].z;
			  z += pow(time, 3) * controlPoints[3].z;
			  z /= 3;
		return z;
	};

	mat4 makeRotationMatrix(vec3 rotate) {
		quat quaterion = quat(rotate);
		return glm::mat4_cast(quaterion);
	};
};
