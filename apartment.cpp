#include "render_stuff.h"
#include "object.h"
#include "camera.h"
#include "blenderModelsHandler.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include <string>
#include <iostream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include "json.hpp"
#include <fstream>

#define SHOW_AXIS 1
#undef SHOW_AXIS


Shader shader;
Shader lightShader;
Shader skyShader;
Camera camera;
std::vector<Object*> objects;
std::vector<fs::path> models_paths;
std::vector<std::string> skybox_paths;
nlohmann::json modelsData;

int GL_WINDOW_HEIGHT = 1000;
int GL_WINDOW_WIDTH = 1000;

void initEnv();
void renderScene();
void processNormalKeys(unsigned char key, int x, int y);
void processMouse(int x, int y);
void loadAXIS();
void loadSky();
void loadPaths();
void loadModelsObj();

int main(int argc, char** argv)
{

	loadPaths();

#pragma region Init GLut

	// init glut, create window
	glutInit(&argc, argv);
	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT);
	glutCreateWindow("Apartment by @timusfed");

	// callbacks
	// todo: reshape
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutKeyboardFunc(processNormalKeys);
	glutPassiveMotionFunc(processMouse);

	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("---------- GLUT init error! ----------");

#pragma endregion

	initEnv();

	glutMainLoop();

	return 0;
}

void initEnv() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glutSetCursor(GLUT_CURSOR_NONE);
	glHint(GL_MULTISAMPLE, GL_NICEST);

	if(!shader.init("shader.vert", "shader.frag")) // initialize shader programs & etc.
		pgr::dieWithError("---------- Standard Shaders init error! ----------");

	if (!lightShader.init("lightShader.vert", "lightShader.frag")) // initialize shader programs & etc.
		pgr::dieWithError("---------- Light Shaders init error! ----------");

	if (!skyShader.init("sky.vert", "sky.frag")) // initialize shader programs & etc.
		pgr::dieWithError("---------- Sky Shaders init error! ----------");

#ifdef SHOW_AXIS
	loadAXIS();
#endif 

	loadSky();

	for (Object* obj : objects) {
		if (obj->isLightsource)
			obj->initModel(shader);
		else if (obj->name == "skybox")
			obj->initModel(skyShader);
		else
			obj->initModel(lightShader);
		std::cout << "--> " << obj->name << " model is initialized\n";
	}

	loadModelsObj();
}

void renderScene() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.4f, 0.8f, 1.0f);

	camera.TimeUpdate();

	for (Object* obj : objects) {
		glm::mat4 projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 view = camera.GetTransfromMatrix();
		glm::mat4 model = glm::translate(glm::mat4(1.0f), obj->position);
		model = glm::scale(model, obj->scale);

		if (obj->isLightsource) { // light source doesn't need reflections
			glUseProgram(shader.program);
			obj->drawObject(shader, view, projection, model); 
		}
		else if (obj->name == "skybox") { // draw sky
			glDepthMask(GL_FALSE);
			glUseProgram(skyShader.program);
			glm::mat4 new_view = glm::mat4(glm::mat3(view));
			skyShader.setTransformUniforms(model, new_view, projection);
			glBindVertexArray(obj->mesh.vao);
			glBindTexture(GL_TEXTURE_CUBE_MAP, obj->mesh.texture);
			glDrawElements(GL_TRIANGLES, obj->countTriangles * 3, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
		}
		else { // standard objects have reflections depending on environment
			glUseProgram(lightShader.program);
			glUniform3fv(lightShader.viewPosLoc, 1, value_ptr(camera.camPosition));
			glUniform3fv(lightShader.lightPosLoc, 1, value_ptr(vec3(-3.7, 1.2, 0.7)));
			obj->drawObject(lightShader, view, projection, model); 
		}
		glUseProgram(0);
	}

	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {
	switch (key)
	{
		case 27: // esc
			glutLeaveMainLoop();
			break;
	}
	glutPostRedisplay();
	camera.OnKeyPress(key);
	// keys table
}

void processMouse(int x, int y) {
	camera.OnMouseMove(x, y);
}

void loadAXIS() {
	// ---- Load X cube ----
	Object* x = new Object(
		"X",
		pgr::cubeData.nAttribsPerVertex,
		pgr::cubeData.nVertices,
		pgr::cubeData.nTriangles,
		pgr::cubeData.verticesInterleaved,
		pgr::cubeData.triangles
	);

	x->position = vec3(1.0, 0.0, 0.0);
	x->scale = vec3(0.05);
	x->mesh.texture = pgr::createTexture("textures/_x.png");
	objects.push_back(x);

	// ---- Load Y cube ----
	Object* y = new Object(
		"Y",
		pgr::cubeData.nAttribsPerVertex,
		pgr::cubeData.nVertices,
		pgr::cubeData.nTriangles,
		pgr::cubeData.verticesInterleaved,
		pgr::cubeData.triangles
	);

	y->position = vec3(0.0, 1.0, 0.0);
	y->scale = vec3(0.05);
	y->mesh.texture = pgr::createTexture("textures/_z.jpg"); // y - z
	objects.push_back(y);

	// ---- Load Z cube ----
	Object* z = new Object(
		"Z",
		pgr::cubeData.nAttribsPerVertex,
		pgr::cubeData.nVertices,
		pgr::cubeData.nTriangles,
		pgr::cubeData.verticesInterleaved,
		pgr::cubeData.triangles
	);

	z->position = vec3(0.0, 0.0, 1.0);
	z->scale = vec3(0.05);
	z->mesh.texture = pgr::createTexture("textures/_y.png");
	objects.push_back(z);
}

void loadPaths() {
	// ---- Json path ----
	std::ifstream src("models_obj/modelsData.json"); // json for models
	src >> modelsData;

	// ---- Models paths ----
	std::string path = fs::current_path().string() + "/models_obj/";
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.path().filename().extension() != ".json") {
			models_paths.push_back(entry.path());
		}
	}

	// ---- Sky paths ----
	path = fs::current_path().string() + "/skybox/";
	for (const auto& entry : fs::directory_iterator(path)) {
		skybox_paths.push_back(entry.path().string());
	}
}

void loadModelsObj() {
	for (fs::path p : models_paths)
	{
		Object* obj = new Object(p.filename().string());
		bool loaded = obj->loadMesh(
			p.string(), 
			lightShader
		);
		if (!loaded)
			pgr::dieWithError("---------- Error while loading the model! ----------");
		else
			std::cout << p.string() << std::endl;

		std::string texFile = modelsData[obj->name]["texture"];
		std::string normFile = modelsData[obj->name]["normal"];

		obj->mesh.texture = pgr::createTexture("textures/" + texFile);
		//obj->mesh.normal = pgr::createTexture("normals/" + normFile);

		float x = modelsData[obj->name]["position"]["x"]; // gl x <- blender x
		float y = modelsData[obj->name]["position"]["z"]; // gl y <- blender z
		float z = modelsData[obj->name]["position"]["y"]; // gl -z <- blender y
		obj->position = vec3(x, y, -z);
		objects.push_back(obj);
	}
}

void loadSky() {
	// ---- Load skybox ----
	Object* skybox = new Object(
		"skybox",
		skyboxNAttribsPerVertex,
		skyboxNVertices,
		skyboxNTriangles,
		skyboxVertices,
		skyboxTriangles
	);
	skybox->scale = vec3(30.0);
	skybox->mesh.texture = loadCubemap(skybox_paths);
	objects.push_back(skybox);

	// ---- Load test light ----
	Object* lamp = new Object(
		"light",
		pgr::cubeData.nAttribsPerVertex,
		pgr::cubeData.nVertices,
		pgr::cubeData.nTriangles,
		pgr::cubeData.verticesInterleaved,
		pgr::cubeData.triangles
	);

	lamp->position = vec3(-3.7, 1.2, 0.7);
	lamp->scale = vec3(0.05);
	lamp->isLightsource = true;
	lamp->mesh.texture = pgr::createTexture("textures/_y.png");
	objects.push_back(lamp);
}