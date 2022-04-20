#include "render_stuff.h"
#include "object.h"
#include "camera.h"
#include "blenderModelsHandler.h"

Shader shader;
Camera camera;
std::vector<Object*> objects;

// screen
int GL_WINDOW_HEIGHT = 600;
int GL_WINDOW_WIDTH = 600;

void initEnv();
void renderScene();
void processNormalKeys(unsigned char key, int x, int y);
void processMouse(int x, int y);

int main(int argc, char** argv)
{

#pragma region Init GLut

	// init glut, create window
	glutInit(&argc, argv);
	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT);
	glutCreateWindow("My first window");

	// callbacks
	// todo: reshape
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutKeyboardFunc(processNormalKeys);
	glutMotionFunc(processMouse);

	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("shit's fuck, man");

#pragma endregion

	initEnv();

	glutMainLoop();

	return 0;
}

void initEnv() {

	glEnable(GL_DEPTH_TEST);

	shader.init(); // initialize shader programs & etc.
	std::cout << "shaders creation complete\n";

	// ---------- test object ----------
	Object* bathroom_wall = new Object(
		bathroom_wallNAttribsPerVertex,
		bathroom_wallNVertices,
		bathroom_wallNTriangles,
		bathroom_wallVertices,
		bathroom_wallTriangles
	);

	bathroom_wall->name = "Bathroom wall";
	bathroom_wall->mesh.texture = pgr::createTexture("skybox_posy.jpg");

	//objects.push_back(bathroom_wall);
	// ---------- ---------- ----------

	// ---------- test object ----------
	Object* pot = new Object(
		pgr::teapotData.nAttribsPerVertex,
		pgr::teapotData.nVertices,
		pgr::teapotData.nTriangles,
		pgr::teapotData.verticesInterleaved,
		pgr::teapotData.triangles
	);

	pot->name = "Teeeeea";
	pot->mesh.texture = pgr::createTexture("skybox_posy.jpg");

	objects.push_back(pot);
	// ---------- ---------- ----------

	for (Object* obj : objects) {
		obj->initModel(shader);
		std::cout << obj->name << " is initialized\n";
	}
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.4f, 0.8f, 1.0f);

	camera.TimeUpdate();

	glm::mat4 projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 view = camera.GetTransfromMatrix();
	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

	for (Object* obj : objects) {
		glUseProgram(shader.program);
		obj->drawObject(shader, view, projection, model);
		glUseProgram(0);;
	}

	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {
	camera.OnKeyPress(key);
	// keys table
}

void processMouse(int x, int y) {
	camera.OnMouseMove(x, y);

	// warp pointer
}


