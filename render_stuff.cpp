#include "render_stuff.h"

void Shader::init() {
	std::vector<GLuint> shaders;

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
	nrmLoc = glGetAttribLocation(program, "normal");
	texLoc = glGetAttribLocation(program, "texCoord");
	PVMLoc = glGetUniformLocation(program, "PVM");

	// mat
	ambtLoc = glGetUniformLocation(program, "material.ambient");
	diffLoc = glGetUniformLocation(program, "material.diffuse");
	specLoc = glGetUniformLocation(program, "material.specular");
	shinLoc = glGetUniformLocation(program, "material.shininess");
	useTextureLoc = glGetUniformLocation(program, "material.useTexture");

	// tex
	texSamplerLoc = glGetUniformLocation(program, "textureSampler");

	CHECK_GL_ERROR();
}

void Shader::setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix));
	return;

	// normal matrix
	const glm::mat4 modelRotationMatrix = glm::mat4(
		modelMatrix[0],
		modelMatrix[1],
		modelMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelRotationMatrix));
	glUniformMatrix4fv(NormMatLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

void Shader::setMaterialUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shiness, GLuint texture) {

	/*
	glUniform3fv(ambtLoc, 1, value_ptr(ambient));
	glUniform3fv(diffLoc, 1, value_ptr(diffuse));
	glUniform3fv(specLoc, 1, value_ptr(specular));
	glUniform1f(shinLoc, shiness);
	*/
	printf("texture: %d\n", texture);
	if (texture != 0)
	{
		glUniform1i(useTextureLoc, 1); // tex sampling
		glUniform1i(texSamplerLoc, 0); // 0 -> samplerID, for GPU linker

		glActiveTexture(GL_TEXTURE2);            // 0 -> to be bound, for OpenGL BindTexture
		glBindTexture(GL_TEXTURE_2D, texture);
		CHECK_GL_ERROR();
	}
	else {
		glUniform1i(useTextureLoc, 0);  // do not sample the texture
	}
}
