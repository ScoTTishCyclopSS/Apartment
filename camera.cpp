#include "camera.h"

extern int  GL_WINDOW_HEIGHT;
extern int  GL_WINDOW_WIDTH;

#define DEFAULT_Y 0.7f


vec3 calculateDirection(float yaw, float pitch) {
	vec3 newDirection;
	newDirection.x = cos(radians(yaw)) * cos(radians(pitch));
	newDirection.y = sin(radians(pitch));
	newDirection.z = sin(radians(yaw)) * cos(radians(pitch));
	return newDirection;
}

Camera::Camera() {

	lastTime = glutGet(GLUT_ELAPSED_TIME);
	camUp = vec3(0.0f, 1.0f, 0.0f);

	positions.push_back(vec3(-4.0f, DEFAULT_Y, -0.9f));
	views.push_back(vec2(6.0, -5.0));

	positions.push_back(vec3(-5.0, 2.2, -3.0));
	views.push_back(vec2(55.0, -25.0));

	positions.push_back(vec3(0.45f, 0.55, 0.5));
	views.push_back(vec2(180.0, 0.0));

	positions.push_back(vec3(1.5f, 2.0, -1.5));
	views.push_back(vec2(150.0, -20.0));

	SetPos(positions[0]);
	SetView(normalize(calculateDirection(views[0].x, views[0].y)));
}

void Camera::SetPos(vec3 pos) {
	camPosition = pos;
}

void Camera::SetView(vec3 view) {
	camFront = view;
}

void Camera::TimeUpdate() {
	float currTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = currTime - lastTime;
	lastTime = currTime;
}

void Camera::OnKeyPress() {

	if (currPos == 0)
		speedEncounter = 0.003f;
	else
		speedEncounter = 0.01f;

	speed = speedEncounter * deltaTime;
	
	if(!lock)
	{ 
		if(callbacks['w'])
			camPosition += speed * camFront;

		if (callbacks['s'])
			camPosition -= speed * camFront;

		if (callbacks['a'])
			camPosition -= speed * normalize(cross(camFront, camUp));

		if (callbacks['d'])
			camPosition += speed * normalize(cross(camFront, camUp));
	}
	// collision
	if (camPosition.x <= -20)
		camPosition.x = -20;
	if (camPosition.x >= 20)
		camPosition.x = 20;

	if (camPosition.y <= -10)
		camPosition.y = -10;
	if (camPosition.y >= 10)
		camPosition.y = 10;

	if (camPosition.z <= -15)
		camPosition.z = -15;
	if (camPosition.z >= 15)
		camPosition.z = 15;
}

void Camera::CheckPos(int key) {
	if (key == 'p')
	{
		currPos += 1;
		if (currPos >= positions.size())
			currPos = 0;

		vec3 newpos = positions[currPos];
		vec2 newview = views[currPos];

		SetPos(newpos);
		SetView(normalize(calculateDirection(newview.x, newview.y)));

		glutPostRedisplay();
	}

	if (currPos == 0) {
		camPosition.y = DEFAULT_Y;
	}
}

void Camera::OnMouseMove(int x, int y) {
	if (!lock)
	{
		float offset_x = x - x_last;
		float offset_y = y_last - y;

		x_last = x;
		y_last = y;

		const float sens = 0.3f;
		offset_x *= sens; // smooth
		offset_y *= sens;

		yaw += offset_x;
		pitch += offset_y;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		if (yaw > 360.0f || yaw < -360.0f)
			yaw = 0.0f;

		camFront = normalize(calculateDirection(yaw, pitch));

		glutPostRedisplay();

		if (x < 20 || x > GL_WINDOW_WIDTH - 20) {
			x_last = GL_WINDOW_WIDTH / 2;
			y_last = GL_WINDOW_HEIGHT / 2;
			glutWarpPointer(GL_WINDOW_WIDTH / 2, GL_WINDOW_HEIGHT / 2);
		}
		if (y < 20 || y > GL_WINDOW_HEIGHT - 20) {
			x_last = GL_WINDOW_WIDTH / 2;
			y_last = GL_WINDOW_HEIGHT / 2;
			glutWarpPointer(GL_WINDOW_WIDTH / 2, GL_WINDOW_HEIGHT / 2);
		}
	}
}

mat4 Camera::GetTransfromMatrix() {
	return lookAt(
		camPosition,
		camPosition + camFront,
		camUp
	);
}