#include "render_stuff.h"

void Shader::init() {
	std::vector<GLuint> shaders;

	// NO LIGHTING YET

	// load & compile simple shader (ONLY colors)
	GLuint VS = pgr::createShaderFromFile(GL_VERTEX_SHADER, "shader.vert");
	GLuint FS = pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shader.frag");

	shaders.push_back(VS);
	shaders.push_back(FS);

	if (shaders[0] == 0 || shaders[1] == 0)
		return;

	program = pgr::createProgram(shaders); // create program with 2 shaders
	if (program == 0) return;

	posLoc = glGetAttribLocation(program, "position");
	rgbLoc = glGetAttribLocation(program, "color");
	PVMLoc = glGetUniformLocation(program, "PVM");

	CHECK_GL_ERROR();
}

void Shader::setTransformUniforms(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
	
	mat4 PVM = projectionMatrix *  viewMatrix *  modelMatrix;

	// <var to save> <count of vectors> <> <>
	glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniformMatrix4fv(CamLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(ModLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// no normal matrix yet
}

void Shader::setMaterialUniforms(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, float shiness, GLuint texture) {
	glUniform3fv(ambtLoc, 1, glm::value_ptr(ambient));
	glUniform3fv(diffLoc, 1, glm::value_ptr(diffuse));
	glUniform3fv(specLoc, 1, glm::value_ptr(specular));
	glUniform1f(shinLoc, shiness);

	if (texture != 0)
	{
		glUniform1i(useTextureLoc, 1); // tex sampling
		glUniform1i(texSamplerLoc, 0); // 0 -> samplerID, for GPU linker
		glActiveTexture(GL_TEXTURE0 + 0);            // 0 -> to be bound, for OpenGL BindTexture
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	else {
		glUniform1i(useTextureLoc, 0);  // do not sample the texture
	}
}
