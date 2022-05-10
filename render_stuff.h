#ifndef __RENDER_STUFF_H
#define __RENDER_STUFF_H

#include <iostream>
#include "pgr.h"

using namespace glm;

class Shader {
public:
	GLuint program; // id
	// ------vertex attribute locations------
	GLuint posLoc; // position 
	GLuint nrmLoc; // normal
	GLuint texLoc; // textrure coords
	GLuint tnLoc; // tangent coords
	GLuint btnLoc; // bitangent coords
	// ------uniforms locations------
	GLuint ViewMatLoc; // view/cam matrix
	GLuint ModMatLoc; // model matrix
	GLuint NormMatLoc; // normal matrix
	GLuint ProjMatLoc; // projection matrix
	// ------ material ------
	GLuint diffLoc;
	GLuint ambtLoc;
	GLuint specLoc;
	GLuint shinLoc;
	// ------ texture ------
	GLuint texSampLoc; // tex sampler loc
	GLuint normSampLoc; // tex sampler loc
	// ------ light ------
	GLuint lightPosLoc; // light position
	GLuint viewPosLoc; // viewPosition

	bool init(std::string vert, std::string frag);

	void setMaterialUniforms(
		const glm::vec3& ambient,
		const glm::vec3& diffuse,
		const glm::vec3& specular,
		float shiness,
		GLuint texture,
		GLuint normal,
		bool isLight
	);

	void setTransformUniforms(
		const glm::mat4& modelMatrix,
		const glm::mat4& viewMatrix,
		const glm::mat4& projectionMatrix
	);
};

GLuint loadCubemap(std::vector<std::string> faces);

#endif // !__RENDER_STUFF_H

