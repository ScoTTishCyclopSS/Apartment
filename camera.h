#pragma once
#include "pgr.h"
#include <iostream>

using namespace glm;

extern int GL_WINDOW_HEIGHT;
extern int GL_WINDOW_WIDTH;

class Camera {
public:
	Camera();

	void SetPos(float x, float y, float z);

	void OnKeyPress(unsigned char key);

	void TimeUpdate();

	void OnMouseMove(int x, int y);

	mat4 transformMatrix;
	mat4 GetTransfromMatrix();

	// camera
	vec3 camPosition;
	vec3 camFront;
	vec3 camUp;

	// calculations
	float speed = 0.05;
	float angle = 0.0;
	float lastTime = 0.0;
	float deltaTime = 0.0; // time between current frame and last frame

	// crosshair
	float x_last = GL_WINDOW_WIDTH / 2;
	float y_last = GL_WINDOW_HEIGHT / 2;
	float yaw = 0.0;
	float pitch = 0.0;
};
