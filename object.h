/**
 * @file object.h
 *
 * @brief Contains the main attributes (classes) for loading, controlling, moving an object on the scene.
 *
 * @ingroup apartment
 *
 * @author Timushev Fedor
 * Contact: timusfed@fel.cvut.cz
 *
 */

#pragma once
#include "render_stuff.h"
#include <iostream>

using namespace glm;

class Mesh {
public:
	GLuint vbo; // vertex buffer object
	GLuint ebo; // element buffer object
	GLuint vao; // vertex attributes object
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

	vec3 position;
	vec3 scale;

	bool isLightsource = false;
	bool isAnimated = false;
	bool isSkybox = false;
	bool isTransparent = false;
	bool isBillboard = false;
	bool isCombined = false;
	bool isGlass = false;

	// default method for loading objects in .obj format
	Object(std::string name_) { // .obj
		name = name_;
		position = vec3(0.0f);
		scale = vec3(1.0f);
		mesh.texture = 0;
		mesh.normal = 0;
	};

	// default method for loading objects in .cpp format
	Object(std::string name_, const int attPerV, const int v_num, const int t_num, const float* v, const unsigned* t) {
		position = vec3(0.0f);
		scale = vec3(1.0f);
		countAttribsPerVertex = attPerV;
		countVertices = v_num;
		countTriangles = t_num;
		vertices = v;
		triangles = t;
		name = name_;
	};

	// initialization method for models defined as .cpp
	void initModel(Shader shader);

	void drawObject(Shader shader, const glm::mat4& viewMat,const glm::mat4& projectMat, const glm::mat4& modelMat);

	// initialization method for models defined as .obj
	bool loadMesh(const std::string& fileName, Shader shader);
};

class MyCurve {
public:
	std::vector<vec3> controlPoints;

	MyCurve(std::vector<vec3> points) {
		controlPoints = points;
	};

	// calculates the current X coord depending on time parametr
	float currX(float time) {
		float x = pow(1 - time, 3) * controlPoints[0].x;
			  x += 3 * time * pow(1 - time, 2) * controlPoints[1].x;
			  x += 3 * pow(time, 2) * (1 - time) * controlPoints[2].x;
			  x += pow(time, 3) * controlPoints[3].x;
			  x /= 3;
		return x;
	};

	// calculates the current Z coord depending on time parametr
	float currZ(float time) {
		float z = pow(1 - time, 3) * controlPoints[0].z;
			  z += 3 * time * pow(1 - time, 2) * controlPoints[1].z;
			  z += 3 * pow(time, 2) * (1 - time) * controlPoints[2].z;
			  z += pow(time, 3) * controlPoints[3].z;
			  z /= 3;
		return z;
	};

	// creates rotation matrix by quaterion
	mat4 makeRotationMatrix(vec3 rotate) {
		quat quaterion = quat(rotate);
		return glm::mat4_cast(quaterion);
	};
};
