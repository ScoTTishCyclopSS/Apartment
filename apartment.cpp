#include "render_stuff.h"
#include "object.h"
#include "camera.h"
#include "blenderModelsHandler.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include <string>
#include <iostream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


Shader shader;
Shader lightShader;
Camera camera;
std::vector<Object*> objects;

// screen
int GL_WINDOW_HEIGHT = 600;
int GL_WINDOW_WIDTH = 600;

void initEnv();
void renderScene();
void processNormalKeys(unsigned char key, int x, int y);
void processMouse(int x, int y);
void loadAXIS();

void drawAxis() {
	float line[] = { 0.0, 0.0, 1.0, 1.0 };
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, 2);
}

int main(int argc, char** argv)
{

	std::string path = fs::current_path().string() + "/models/";
	for (const auto& entry : fs::directory_iterator(path))
		std::cout << entry.path() << std::endl;

	return 1;

#pragma region Init GLut

	// init glut, create window
	glutInit(&argc, argv);
	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
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
		pgr::dieWithError("---------- GLUT init error! ----------");
	std::cout << "++++++++++ GLUT init successful! ++++++++++\n\n";

#pragma endregion

	initEnv();

	glutMainLoop();

	return 0;
}

void initEnv() {

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE, GL_NICEST);

	if(!shader.init("shader.vert", "shader.frag")) // initialize shader programs & etc.
		pgr::dieWithError("---------- Shaders init error! ----------");
	std::cout << "++++++++++ Shaders init successful! ++++++++++\n\n";

	if (!lightShader.init("lightShader.vert", "lightShader.frag")) // initialize shader programs & etc.
		pgr::dieWithError("---------- Light Shaders init error! ----------");
	std::cout << "++++++++++ Light Shaders init successful! ++++++++++\n\n";


	// ---------- ---------- ----------

	Object *wall = new Object("Wall");
	bool sas = wall->loadMesh(
		"G:/StudyCTU/4th_semestr/PGR/apartment/main_wall.obj", 
		lightShader.posLoc, 
		lightShader.nrmLoc, 
		&wall->mesh.vbo, 
		&wall->mesh.ebo, 
		&wall->mesh.vao, 
		&wall->countTriangles
	);

	//Object* wall = new Object(
	//	"Wall",
	//	main_wallNAttribsPerVertex,
	//	main_wallNVertices,
	//	main_wallNTriangles,
	//	main_wallVertices,
	//	main_wallTriangles
	//);

	wall->mesh.texture = pgr::createTexture("textures/blue.png");
	
	//material
	//wall->material.ambient = vec3(1.0f, 0.0f, 1.0f);
	//wall->material.diffuse = vec3(1.0f, 0.0f, 1.0f);
	//wall->material.specular = vec3(1.0f, 0.0f, 1.0f);
	//wall->material.shiness = 10.0f;

	objects.push_back(wall);

	return;

	// ---------- ---------- ----------

	loadAXIS();


	for (Object* obj : objects) {
			if (obj->isLightsource)
				obj->initModel(shader);
			else
				obj->initModel(lightShader);
			std::cout << "--> " << obj->name << " model is initialized\n";
	}
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.4f, 0.8f, 1.0f);

	drawAxis();

	camera.TimeUpdate();

	for (Object* obj : objects) {
		if (obj->isLightsource)
			glUseProgram(shader.program);
		else
			glUseProgram(lightShader.program);

		glm::mat4 projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 view = camera.GetTransfromMatrix();
		glm::mat4 model = glm::translate(glm::mat4(1.0f), obj->position);
		model = glm::scale(model, obj->scale);
		 
		if (obj->isLightsource)
			obj->drawObject(shader, view, projection, model);
		else
			obj->drawObject(lightShader, view, projection, model);
		
		glUseProgram(0);
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

void loadAXIS() {
	Object* x = new Object(
		"X",
		pgr::cubeData.nAttribsPerVertex,
		pgr::cubeData.nVertices,
		pgr::cubeData.nTriangles,
		pgr::cubeData.verticesInterleaved,
		pgr::cubeData.triangles
	);

	x->position = vec3(3.0, 0.0, 0.0);
	x->scale = vec3(0.05);
	x->mesh.texture = pgr::createTexture("textures/x.png");
	objects.push_back(x);

	Object* y = new Object(
		"Y",
		pgr::cubeData.nAttribsPerVertex,
		pgr::cubeData.nVertices,
		pgr::cubeData.nTriangles,
		pgr::cubeData.verticesInterleaved,
		pgr::cubeData.triangles
	);

	y->position = vec3(0.0, 3.0, 0.0);
	y->scale = vec3(0.05);
	y->mesh.texture = pgr::createTexture("textures/y.png");
	objects.push_back(y);

	Object* z = new Object(
		"Z",
		pgr::cubeData.nAttribsPerVertex,
		pgr::cubeData.nVertices,
		pgr::cubeData.nTriangles,
		pgr::cubeData.verticesInterleaved,
		pgr::cubeData.triangles
	);

	z->position = vec3(0.0, 0.0, 3.0);
	z->scale = vec3(0.05);
	z->mesh.texture = pgr::createTexture("textures/z.png");
	objects.push_back(z);
}
