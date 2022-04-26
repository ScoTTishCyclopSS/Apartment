#include "camera.h"

extern int  GL_WINDOW_HEIGHT;
extern int  GL_WINDOW_WIDTH;

Camera::Camera() {
	camPosition = vec3(-2.0f, 0.0f, 0.0f); // origin
	camTarget = vec3(0.0f, 0.0f, 0.0f); // target
	up = vec3(0.0f, 0.0f, 1.0f); // align with z

	camDirection = normalize(camPosition - camTarget); // what direction it is pointing at
	camRight = normalize(cross(up, camDirection)); // positive x-axis of the camera space
	camUp = cross(camDirection, camRight); // points to the camera’s positive y - axis

	camFront = vec3(0.0f, 0.0f, -1.0f);
	lastTime = glutGet(GLUT_ELAPSED_TIME);
}

void Camera::SetPos(float x, float y, float z) {
	camPosition.x = x;
	camPosition.y = y;
	camPosition.z = z;
}

void Camera::TimeUpdate() {
	float currTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = currTime - lastTime;
	lastTime = currTime;
}

void Camera::OnKeyPress(unsigned char key) {

	speed = 0.01f * deltaTime;
	
	switch (key)
	{
	case 'w':
	{
		camPosition += speed * camFront;
		break;
	}
	case 's':
		camPosition -= speed * camFront;
		break;
	case 'a':
	{
		camPosition -= speed * normalize(cross(camFront, camUp));
		break;
	}
	case 'd':
	{
		camPosition += speed * normalize(cross(camFront, camUp));
		break;
	}
	default:
		break;
	}
}

void Camera::OnMouseMove(int x, int y) {
	float offset_x = x - x_last;
	float offset_y = y_last - y;

	x_last = x;
	y_last = y;

	const float sens = 0.1f;
	offset_x *= sens; // smooth
	offset_y *= sens;

	yaw += offset_y;
	pitch -= offset_x;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	vec3 newDirection;
	newDirection.x = cos(radians(yaw)) * cos(radians(pitch));
	newDirection.y = sin(radians(pitch));
	newDirection.z = sin(radians(yaw)) * cos(radians(pitch));

	camFront = normalize(newDirection);

	glutPostRedisplay();

	if (x < 100 || x > GL_WINDOW_WIDTH - 100) {  
		x_last = GL_WINDOW_WIDTH / 2;
		y_last = GL_WINDOW_HEIGHT / 2;
		glutWarpPointer(GL_WINDOW_WIDTH / 2, GL_WINDOW_HEIGHT / 2);
	}
	else if (y < 100 || y > GL_WINDOW_HEIGHT - 100) {
		x_last = GL_WINDOW_WIDTH / 2;
		y_last = GL_WINDOW_HEIGHT / 2;
		glutWarpPointer(GL_WINDOW_WIDTH / 2, GL_WINDOW_HEIGHT / 2);
	}
}

mat4 Camera::GetTransfromMatrix() {
	return lookAt(
		camPosition,
		camPosition + camFront,
		camUp
	);
}