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

	nrmLoc = 1;
	texLoc = 2;

	printf("%d\n", texLoc);
	

	if (lighting) {

		// mesh
		PVMLoc = glGetUniformLocation(program, "PVM");
		ViewMatLoc = glGetUniformLocation(program, "viewMat");
		ModMatLoc = glGetUniformLocation(program, "modelMat");
		NormMatLoc = glGetUniformLocation(program, "normMat");

		// mat
		ambtLoc = glGetUniformLocation(program, "material.ambient");
		diffLoc = glGetUniformLocation(program, "material.diffuse");
		specLoc = glGetUniformLocation(program, "material.specular");
		shinLoc = glGetUniformLocation(program, "material.shininess");

		// tex
		texSamplerLoc = glGetUniformLocation(program, "texSampler");
		useTextureLoc = glGetUniformLocation(program, "ourTexture");
	}

	CHECK_GL_ERROR();
}

void Shader::setTransformUniforms(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
	
	mat4 PVM = projectionMatrix *  viewMatrix *  modelMatrix;

	// <var to save> <count of vectors> <> <>
	glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniformMatrix4fv(ViewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(ModMatLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

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

void Shader::setMaterialUniforms(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, float shiness, GLuint texture) {
	glUniform3fv(ambtLoc, 1, value_ptr(ambient));
	glUniform3fv(diffLoc, 1, value_ptr(diffuse));
	glUniform3fv(specLoc, 1, value_ptr(specular));
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
