/**
 * @file camera.h
 *
 * @brief Main camera header files. Contents camera class.
 *
 * @ingroup apartment
 *
 * @author Timushev Fedor
 * Contact: timusfed@fel.cvut.cz
 *
 */
#pragma once
#include "pgr.h"
#include <iostream>

using namespace glm;

extern int GL_WINDOW_HEIGHT;
extern int GL_WINDOW_WIDTH;

class Camera {
public:
	Camera();

	void SetPos(vec3 pos);

	void SetView(vec3 view);

	void OnKeyPress();

	void TimeUpdate();

	void CheckPos(int key);

	void OnMouseMove(int x, int y);

	mat4 transformMatrix;
	mat4 GetTransfromMatrix();

	vec3 camPosition;
	vec3 camFront;
	vec3 camUp;

	float speed = 0.01;
	float angle = 0.0;
	float lastTime = 0.0;
	float deltaTime = 0.0; // time between current frame and last frame
	float timeOnMove = 0.0;
	float speedEncounter = 0.01;

	// crosshair
	float x_last = GL_WINDOW_WIDTH / 2;
	float y_last = GL_WINDOW_HEIGHT / 2;
	float yaw = 0.0;
	float pitch = 0.0;

	bool lock = false;
	int currPos = 0;

	std::vector<vec3> positions;
	std::vector<vec2> views; // yaw, pitch

	bool callbacks[256] = { false };
};
