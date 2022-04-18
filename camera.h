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
	vec3 camTarget;
	vec3 camDirection;
	vec3 up;
	vec3 camRight;
	vec3 camUp;
	vec3 camFront;

	// calculations
	float speed = 1.0;
	float angle = 0.0;
	float lastTime = 0.0;
	float deltaTime = 0.0; // time between current frame and last frame

	// crosshair
	float x_last = GL_WINDOW_WIDTH / 2;
	float y_last = GL_WINDOW_HEIGHT / 2;
	float yaw = -90.0;
	float pitch = 0.0;
};
