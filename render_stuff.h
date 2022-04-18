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
	GLuint rgbLoc; // color
	GLuint nrmLoc; // normal
	GLuint texLoc; // textrure coords
	// ------uniforms locations------
	GLuint PVMLoc; // pvm matrix
	GLuint CamLoc; // view/cam matrix
	GLuint ModLoc; // model matrix
	// ------ material ------
	GLuint diffLoc;
	GLuint ambtLoc;
	GLuint specLoc;
	GLuint shinLoc;
	// ------ texture ------
	GLuint useTextureLoc;
	GLuint texSamplerLoc;
	// ------ light ------
	bool lighting = false;

	void init();

	void setMaterialUniforms(
		const glm::vec3& ambient,
		const glm::vec3& diffuse,
		const glm::vec3& specular,
		float shiness,
		GLuint texture
	);

	void setTransformUniforms(
		const glm::mat4& modelMatrix,
		const glm::mat4& viewMatrix,
		const glm::mat4& projectionMatrix
	);
};

#endif // !__RENDER_STUFF_H

