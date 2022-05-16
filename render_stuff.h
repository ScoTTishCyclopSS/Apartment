#ifndef __RENDER_STUFF_H
#define __RENDER_STUFF_H

#include <iostream>
#include "pgr.h"
#include "light.h"

using namespace glm;

struct pointLight
{
	GLuint diffPos;
	GLuint ambPos;
	GLuint specPos;

	GLuint conPos;
	GLuint linPos;
	GLuint quadPos;

	GLuint blinPos;
	GLuint posPos;
};

struct dirLight
{
	GLuint diffPos;
	GLuint ambPos;
	GLuint specPos;

	GLuint direction;
};

struct spotLight
{
	GLuint diffPos;
	GLuint ambPos;
	GLuint specPos;

	GLuint direction;
};

struct Fog {
	vec3 color;
	float start, end, density;
	int type;
	bool isActive;
};

class Shader {
public:
	GLuint program; // id
	// ---- vertex attribute locations ----
	GLuint posLoc; // position 
	GLuint nrmLoc; // normal
	GLuint uvLoc; // textrure coords
	GLuint tnLoc; // tangent 
	GLuint btnLoc; // bitangent 
	GLuint nextPosLoc; // bones with influence to vertex
	GLuint tPos;

	// ---- uniforms locations ----
	GLuint ViewMatLoc; // view/cam matrix
	GLuint ModMatLoc; // model matrix
	GLuint NormMatLoc; // normal matrix
	GLuint ProjMatLoc; // projection matrix
	GLuint FinBonesLoc; // final bones matricies

	// ---- texture ----
	GLuint texSampLoc; // tex sampler loc
	GLuint normSampLoc; // tex sampler loc
	GLuint roughSampLoc; // tex sampler loc

	// ---- light ----
	GLuint viewPosLoc; // viewPosition

	// ---- point light ----
	pointLight pointLightProps;
	dirLight dirLightProps;
	spotLight spotLightProps;

	// ---- Fog ----
	Fog fogProps;

	// ---- Water ----
	GLuint waveStrengthLoc;
	GLuint layerLoc;

	bool init(std::string vert, std::string frag);

	void setMaterialUniforms(GLuint texture, GLuint normal, GLuint rough);

	void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	void setPointLightUniforms(std::vector<PointLight*> pointLights);

	void setFogUniforms(vec3 color, float start, float end, float dens, int type, bool active);

	void setSpotLightUniforms(SpotLight* spl, bool active);

	void setDirLightUniforms(DirectLight* dl);
};

GLuint loadCubemap(std::vector<std::string> faces);

#endif // !__RENDER_STUFF_H

