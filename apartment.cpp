#include "render_stuff.h"
#include "object.h"
#include "camera.h"
#include "models_cpp/skybox.h"
#include "models_cpp/plane.h"
#include "models_cpp/roof.h"
#include "json.hpp"
#include "gameObjects.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include <string>
#include <iostream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#include <fstream>

int GL_WINDOW_HEIGHT = 800;
int GL_WINDOW_WIDTH = 800;

#define REFRESH_INTERVAL 33

using namespace std;

struct Scene {
	nlohmann::json modelsData;

	Shader shader;
	Shader lightShader;
	Shader skyShader;
	Shader waterShader;
	Camera camera;
	float previousTime = 0;
	float t = 0.0;
	float fov = 70.0;

	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;

	bool fog_type = true;
	bool isFog = true;

	bool reflector = false;
	bool direct = false;

	bool wireframe = false;

	float waveStrength = 0.2;
	float layer = 0.5;

	vector<Object*> objects;
	vector<Object*> transObjects;
	vector<Object*> point_lights_obj;
	vector<fs::path> models_paths;
	vector<string> skybox_paths;

	// lights
	vector<PointLight*> point_lights;
	SpotLight* flashlight;
	DirectLight* dirLight;
};

void initEnv();
void renderScene();
void processNormalKeys(unsigned char key, int x, int y);
void processNormalKeysUp(unsigned char key, int x, int y);
void processMouseClicker(int button, int state, int x, int y);
void processMouse(int x, int y);
void loadModelsCpp();
void loadPaths();
void loadModelsObj();
void loadLights();
void actionOnClickedObj(Object* clickedObj);
void onTime(int);

Scene scene;
CoffeeMake coffeeMake;
Window window;
Lamp lamp;
Robot robot;

int main(int argc, char** argv)
{
	loadPaths();

#pragma region Glut_init

	// init glut, create window
	glutInit(&argc, argv);
	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STENCIL);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT);
	glutCreateWindow("Apartment by @timusfed");

	// callbacks
	// todo: reshape
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(processNormalKeysUp);
	glutPassiveMotionFunc(processMouse);
	glutMouseFunc(processMouseClicker);
	glutTimerFunc(REFRESH_INTERVAL, onTime, 0);

	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("---------- GLUT init error! ----------");

#pragma endregion

	initEnv();

	glutMainLoop();

	return 0;
}

void initEnv() {
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);

	// multisampling
	glHint(GL_MULTISAMPLE, GL_NICEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	// blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);

#pragma region Shaders_init

	if(!scene.shader.init("shaders/shader.vert", "shaders/shader.frag"))
		pgr::dieWithError("---------- Standard Shader init error! ----------");

	if (!scene.lightShader.init("shaders/lightShader.vert", "shaders/lightShader.frag"))
		pgr::dieWithError("---------- Light Shader init error! ----------");

	if (!scene.skyShader.init("shaders/sky.vert", "shaders/sky.frag"))
		pgr::dieWithError("---------- Sky Shader init error! ----------");

	if (!scene.waterShader.init("shaders/waterShader.vert", "shaders/waterShader.frag"))
		pgr::dieWithError("---------- Water Shader init error! ----------");

#pragma endregion

	loadModelsCpp();

	loadLights();

	// .cpp meshes init
	// depending on type
	for (Object* obj : scene.objects) {
		if (obj->isLightsource)
			obj->initModel(scene.shader);
		else if (obj->isSkybox)
			obj->initModel(scene.skyShader);
		else
			obj->initModel(scene.lightShader);
	}

	loadModelsObj();

	// push transparent objects at the end of objects array
	// they need to be draw last
	for (Object* transObj : scene.transObjects) {
		if (transObj->isTransparent) {
			transObj->initModel(scene.waterShader);
			scene.objects.push_back(transObj);
		}
	}

	scene.transObjects.clear();

	// make a robot (define curve and rotation quat
	vector<vec3> controlPoints = { vec3(0.9, 0.0, 0.5), vec3(0.5, 0.0, 0.0), vec3(-0.5, 0.0, 0.0), vec3(-0.5, 0.0, -1.0) };
	robot.coons = new MyCurve(controlPoints);
}

void renderScene() {

	if(scene.wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene.camera.TimeUpdate();
	scene.camera.OnKeyPress();

	for (Object* obj : scene.objects)
	{
		scene.projection = glm::perspective(glm::radians(scene.fov), 4.0f / 3.0f, 0.01f, 100.0f);
		scene.view = scene.camera.GetTransfromMatrix();
		scene.model = glm::scale(glm::translate(glm::mat4(1.0f), obj->position), obj->scale);

		// my poor try to pick a cup
		if (coffeeMake.cupPicked && (obj == coffeeMake.cup || obj == coffeeMake.fill))
		{
			vec3 newPos = scene.camera.camPosition + glm::normalize(scene.camera.camFront) * 0.05f;
			scene.model = glm::scale(glm::translate(glm::mat4(1.0f), newPos), obj->scale);
		}

		// don't draw roof if the camera is not inside the house
		if (scene.camera.currPos != 0 && obj->name == "roof")
			continue;

		// change robot position with the coons
		if (obj->name == "bedroom_robot.obj") {
			obj->position = vec3(robot.coons->currX(robot.t2) - 1.0, obj->position.y, robot.coons->currZ(robot.t2) - 0.5);
			vec3 angles = vec3(0.0, scene.t * 45, 0.0);
			scene.model *= robot.coons->makeRotationMatrix(angles);
		}
			
		if (obj->isLightsource) {
			glUseProgram(scene.shader.program);
			obj->drawObject(scene.shader, scene.view, scene.projection, scene.model);
		}
		else if (obj->isSkybox) {
			glUseProgram(scene.skyShader.program);
			obj->drawObject(scene.skyShader, scene.view, scene.projection, scene.model);
		}
		else if (obj->isTransparent) {

			if (!coffeeMake.filling && obj == coffeeMake.coffeeFlow)
				continue;

			glUseProgram(scene.waterShader.program);
			glUniform1f(scene.waterShader.tPos, scene.t);

			if (obj == coffeeMake.fill || obj == coffeeMake.coffeeFlow)
			{
				glUniform1f(scene.waterShader.waveStrengthLoc, coffeeMake.waveStrength);
				glUniform1f(scene.waterShader.layerLoc, coffeeMake.layer);
			}
			else
			{
				glUniform1f(scene.waterShader.waveStrengthLoc, scene.waveStrength);
				glUniform1f(scene.waterShader.layerLoc, scene.layer);
			}
			
			glUniform1i(glGetUniformLocation(scene.waterShader.program, "glass"), obj->isGlass);

			obj->drawObject(scene.waterShader, scene.view, scene.projection, scene.model);
		}
		else { 
			glUseProgram(scene.lightShader.program);

			// skip tangent space for roof because it's a hardcode object
			if (obj->name == "roof")
				glUniform1i(glGetUniformLocation(scene.lightShader.program, "skipTangent"), 1);
			else
				glUniform1i(glGetUniformLocation(scene.lightShader.program, "skipTangent"), 0);

			// optional - skip directional light (because it's a closed place)
				glUniform1i(glGetUniformLocation(scene.lightShader.program, "useDirLight"), scene.direct);

			// send directional light
			scene.lightShader.setDirLightUniforms(scene.dirLight);

			// send fog to shaders
			scene.lightShader.setFogUniforms(vec3(0.7), 0.5, 2.0, 0.1, scene.fog_type, scene.isFog);

			// send point lights light
			glUniform1i(glGetUniformLocation(scene.lightShader.program, "lampIsOn"), lamp.active);
			scene.lightShader.setPointLightUniforms(scene.point_lights);

			// send spotlight (if active) to shaders
			scene.flashlight->direction = scene.camera.camFront;
			scene.flashlight->position = scene.camera.camPosition;
			scene.lightShader.setSpotLightUniforms(scene.flashlight, scene.reflector);

			// send billboard info
			if (obj->isBillboard)
			{
				vec3 objToCamProj = scene.camera.camPosition - obj->position;
				objToCamProj.y = 0.0;

				objToCamProj = normalize(objToCamProj);

				vec3 lookAt = vec3(0.0, 0.0, 1.0);

				float rightAngle = glm::dot(lookAt, objToCamProj); // inner 
				vec3 up = glm::cross(lookAt, objToCamProj);

				if (abs(rightAngle) < 1)
					scene.model = glm::rotate(scene.model, acos(rightAngle), up);
			}

			if (obj->isCombined)
			{
				glUniform1f(glGetUniformLocation(scene.lightShader.program, "t"), scene.previousTime);
				glUniform1i(glGetUniformLocation(scene.lightShader.program, "combinedTexture"), 1);
			}

			glUniform3fv(scene.lightShader.viewPosLoc, 1, value_ptr(scene.camera.camPosition));
			obj->drawObject(scene.lightShader, scene.view, scene.projection, scene.model);

			if (obj->isCombined)
			{
				glUniform1i(glGetUniformLocation(scene.lightShader.program, "combinedTexture"), 0);
			}
		}

		glUseProgram(0);
	}

	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {
	switch (key)
	{
		case 27: // esc -> close app
			glutLeaveMainLoop();
			break;

		case 'l': // l -> camera lock
			scene.camera.lock = !scene.camera.lock;
			break;

		case 'f': // f -> fog on/off
			scene.isFog = !scene.isFog;
			break;

		case 't': // t -> fog type
			scene.fog_type = !scene.fog_type;
			break;

		case 'r': // r -> reflector on/off
			scene.reflector = !scene.reflector;
			break;

		case 'e': // e -> directional light on/off
			scene.direct = !scene.direct;
			break;

		case 'm': // m -> wireframe on/off
			scene.wireframe = !scene.wireframe;
			break;

		case 'p':
			scene.camera.CheckPos('p');
			break;

	}
	scene.camera.callbacks[key] = true;
	// keys table
}

void processNormalKeysUp(unsigned char key, int x, int y) {
	scene.camera.callbacks[key] = false;
}

void processSelection(int x, int y) {
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(scene.lightShader.program);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	for (int i = 0; i < scene.objects.size(); i++) {

		glm::mat4 projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 0.01f, 100.0f);
		glm::mat4 view = scene.camera.GetTransfromMatrix();
		glm::mat4 model = glm::translate(glm::mat4(1.0f), scene.objects[i]->position);
		model = glm::scale(model, scene.objects[i]->scale);

		glBindVertexArray(scene.objects[i]->mesh.vao);
		glStencilFunc(GL_ALWAYS, i, -1);

		scene.objects[i]->drawObject(scene.lightShader, view, projection, model);
	}

	glBindVertexArray(0);

	glFinish();

	GLbyte color[4];
	GLfloat depth;
	GLuint index;

	glReadPixels(x, GL_WINDOW_HEIGHT - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
	glReadPixels(x, GL_WINDOW_HEIGHT - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	glReadPixels(x, GL_WINDOW_HEIGHT - y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

	printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u, ",
		x, y, color[0], color[1], color[2], color[3], depth, index);

	std::cout << "name selected: " << scene.objects[index]->name << std::endl;
	actionOnClickedObj(scene.objects[index]);
}

void actionOnClickedObj(Object *clickedObj) {
	if (clickedObj == coffeeMake.coffeeMachine) {
		coffeeMake.filling = true;
	}
	if (clickedObj == coffeeMake.cup && coffeeMake.pickebleCup)
	{
		coffeeMake.cupPicked = true;
	}
	if (clickedObj == lamp.lamp)
	{
		lamp.active = !lamp.active;
	}
	if (clickedObj == window.window && !window.opening)
	{
		window.opening = true;
	}
}

void processMouseClicker(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		processSelection(x, y);
		glutPostRedisplay();
	}
}

void processMouse(int x, int y) {
	if (!scene.camera.lock)
		scene.camera.OnMouseMove(x, y);
}

void loadPaths() {
	// ---- Json path ----
	std::ifstream src("models_obj/modelsData.json"); // json for models
	src >> scene.modelsData;

	// ---- Models paths ----
	std::string path = fs::current_path().string() + "/models_obj/";
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.path().filename().extension() != ".json") {
			scene.models_paths.push_back(entry.path());
		}
	}

	// ---- Skybox paths ----
	path = fs::current_path().string() + "/textures/skybox/";
	for (const auto& entry : fs::directory_iterator(path)) {
		scene.skybox_paths.push_back(entry.path().string());
	}
}

void loadModelsObj() {
	for (fs::path p : scene.models_paths)
	{
		Object* obj = new Object(p.filename().string());
		bool loaded = obj->loadMesh(p.string(), scene.lightShader);

		if (!loaded)
			pgr::dieWithError("---------- Error while loading the model! ----------");

		std::string texFile = scene.modelsData[obj->name]["texture"];

		obj->mesh.texture = pgr::createTexture("textures/diffuse/" + texFile);
		obj->mesh.normal = pgr::createTexture("textures/normals/" + texFile);
		obj->mesh.rough = pgr::createTexture("textures/roughness/" + texFile);

		float x = scene.modelsData[obj->name]["position"]["x"]; // gl x <- blender x
		float y = scene.modelsData[obj->name]["position"]["z"]; // gl y <- blender z
		float z = scene.modelsData[obj->name]["position"]["y"]; // gl -z <- blender y
		obj->position = vec3(x, y, -z);

#pragma region Coffee_make
		if (obj->name == "kitchen_coffee.obj")
			coffeeMake.coffeeMachine = obj;
		if (obj->name == "kitchen_coffee_flow.obj") {
			coffeeMake.coffeeFlow = obj;
			coffeeMake.coffeeFlow->mesh.normal = pgr::createTexture("textures/water/water.jpg");
			coffeeMake.coffeeFlow->mesh.rough = pgr::createTexture("textures/water/water.png");
			coffeeMake.coffeeFlow->mesh.texture = pgr::createTexture("textures/water/coffee.png");
			coffeeMake.coffeeFlow->isTransparent = true;
		}
		else if (obj->name == "kitchen_cup.obj")
			coffeeMake.cup = obj;
		else if (obj->name == "kitchen_cup_fill.obj") {
			coffeeMake.fill = obj;
			coffeeMake.fill->mesh.normal = pgr::createTexture("textures/water/water.jpg");
			coffeeMake.fill->mesh.rough = pgr::createTexture("textures/water/water.png");
			coffeeMake.fill->mesh.texture = pgr::createTexture("textures/water/coffee.png");
			coffeeMake.fill->position = coffeeMake.cup->position;
			coffeeMake.fill->position.x += 0.005; // little correction...)
			coffeeMake.fill->position.y -= 0.02; // little correction...)
			coffeeMake.fillYpos_old = coffeeMake.fill->position.y;
			coffeeMake.fill->isTransparent = true;
		}
#pragma endregion

#pragma region Billboard
		// billboard
		else if (obj->name == "kitchen_plant_flower.obj")
		{
			obj->isBillboard = true;
			obj->scale = vec3(1.3);
			obj->position.y += 0.01;
		}
#pragma endregion

#pragma region Pc_dynam_tex
		else if (obj->name == "bedroom_pc.obj")
		{
			obj->mesh.texture = pgr::createTexture("textures/combined/" + texFile);
			obj->isCombined = true;
		}
#pragma endregion

#pragma region Window
		else if (obj->name == "window2.obj")
		{
			window.window = obj;
			window.oldPosY = obj->position.y;
		}
			
		else if (obj->name == "window_glass.obj" || obj->name == "window2_glass.obj") {
			if (obj->name == "window2_glass.obj")
				window.glass = obj;
			obj->isTransparent = true;
			obj->isGlass = true;
		}
#pragma endregion

		else if (obj->name == "bedroom_lamp.obj")
			lamp.lamp = obj;

		scene.objects.push_back(obj);
	}

}

void loadModelsCpp() {
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
	skybox->mesh.texture = loadCubemap(scene.skybox_paths);
	skybox->isSkybox = true;
	scene.objects.push_back(skybox);

	// ---- Load roof (hardcode 30+ triangles) ----
	Object* roof = new Object(
		"roof",
		roofNAttribsPerVertex,
		roofNVertices,
		roofNTriangles,
		roofVertices,
		roofTriangles
	);
	roof->mesh.texture = pgr::createTexture("textures/diffuse/roof.png");
	scene.objects.push_back(roof);

	// ---- Load water plane ----
	Object* plane = new Object(
		"water",
		planeNAttribsPerVertex,
		planeNVertices,
		planeNTriangles,
		planeVertices,
		planeTriangles
	);
	plane->mesh.normal = pgr::createTexture("textures/water/water.jpg");
	plane->mesh.rough = pgr::createTexture("textures/water/water.png");
	plane->mesh.texture = pgr::createTexture("textures/diffuse/_z.png");
	plane->isTransparent = true;
	scene.transObjects.push_back(plane); // push it into transparent objects array
}

void loadLights() {

#pragma region Point_lights

	scene.point_lights.push_back(
		new PointLight(
			vec3(0.8),vec3(0.05),vec3(0.03),
			1.0,0.09,0.032,0,
			vec3(-3.8, 1.25, -0.75),
			vec3(-1.7, 1.25, -0.75)
		)
	);

	scene.point_lights.push_back(
		new PointLight(
			vec3(0.8),vec3(0.05), vec3(0.03),
			1.0,0.09,0.032,0,
			vec3(-3.8, 1.25, 0.35),
			vec3(-3.8, 1.25, 1.2)
		)
	);

	scene.point_lights.push_back(
		new PointLight(
			vec3(0.8), vec3(0.05), vec3(0.03),
			1.0, 0.09, 0.032, 0,
			vec3(-0.65, 1.25, -0.75),
			vec3(-0.65, 1.25, 0.8)
		)
	);

	scene.point_lights.push_back( // lamp light
		new PointLight(
			vec3(0.4), vec3(0.05), vec3(0.05),
			0.8, 0.07, 0.032, 0,
			vec3(-0.19, 0.66, 1.347),
			vec3(-0.19, 0.86, 1.347)
		)
	);

	for (int i = 0; i < scene.point_lights.size() - 1; i++)
	{
		Object* light = new Object(
			"light",
			pgr::cubeData.nAttribsPerVertex,
			pgr::cubeData.nVertices,
			pgr::cubeData.nTriangles,
			pgr::cubeData.verticesInterleaved,
			pgr::cubeData.triangles
		);

		light->position = scene.point_lights[i]->position;
		light->scale = vec3(0.03);
		light->isLightsource = true;
		scene.objects.push_back(light);
		scene.point_lights_obj.push_back(light); // push meshes which will represent light sources
	}

#pragma endregion

	scene.flashlight = new SpotLight(vec3(0.8), vec3(0.5), vec3(0.2), scene.camera.camPosition, scene.camera.camFront, glm::cos(glm::radians(10.0f)));

	scene.dirLight = new DirectLight(vec3(0.9), vec3(0.01), vec3(0.01), vec3(-10.0, 10.0, 16.0));
}

void onTime(int) {

	// ---- Moving Lights ----
	glutTimerFunc(REFRESH_INTERVAL, onTime, 0); // 33 = refresh interval
	float elapsedTime = 0.001 * ((float)glutGet(GLUT_ELAPSED_TIME) - scene.previousTime); // milliseconds => seconds
	scene.previousTime = elapsedTime;

	for (int i = 0; i < scene.point_lights.size() - 1; i++)
		scene.point_lights[i]->position = mix(scene.point_lights[i]->startPosition, scene.point_lights[i]->interpolateTo, abs(cos((elapsedTime))));

	for (int i = 0; i < scene.point_lights_obj.size(); i++)
		scene.point_lights_obj[i]->position = scene.point_lights[i]->position;

	// ---- Normalized by cos elapsed time ---- 
	scene.t = abs(cos((elapsedTime))) * 0.1;
	
	// calculate another time parametr (for robot)
	if (robot.t2 >= 1.0)
		robot.backtrack = true;
	if (robot.t2 <= -1.0)
		robot.backtrack = false;
	robot.t2 += robot.backtrack ? -0.005 : 0.005;

	// ---- Coffee stuff ----
	if (coffeeMake.filling) {
		coffeeMake.fill->position.y += 0.001 * scene.t;
		if (coffeeMake.fill->position.y >= coffeeMake.fillYpos_old + 0.016)
		{
			coffeeMake.filling = false;
			coffeeMake.pickebleCup = true;
		}
	}

	// window movement
	if (window.opening)
	{
		if (window.opened) {
			window.window->position.y -= 0.05 * scene.t;
			window.glass->position.y -= 0.05 * scene.t;
			if (window.window->position.y <= window.oldPosY)
			{
				window.opening = false;
				window.opened = false;
			}
		}
		else {
			window.window->position.y += 0.05 * scene.t;
			window.glass->position.y += 0.05 * scene.t;
			if (window.window->position.y >= window.oldPosY + 0.3)
			{
				window.opening = false;
				window.opened = true;
			}
		}
	}



	glutPostRedisplay();
}