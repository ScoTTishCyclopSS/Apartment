#include "render_stuff.h"

bool Shader::init(std::string vert, std::string frag) {
	std::vector<GLuint> shaders;

	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, vert));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, frag));

	if (shaders[0] == 0 || shaders[1] == 0) {
		std::cout << "---------- Shaders creation error! ----------\n";
		return false;
	}
	std::cout << "++++++++++ Shaders creation successful! ++++++++++\n\n";
		
	program = pgr::createProgram(shaders); // create program with 2 shaders
	if (program == 0) {
		std::cout << "---------- Shaders program creation error! ----------\n";
		return false;
	}
	std::cout << "++++++++++ Shaders program creation successful! ++++++++++\n\n";

	// get vertex attributes locations
	posLoc = 0;
	nrmLoc = 1;
	texLoc = 2;

	// get uniforms locations
	PVMLoc = glGetUniformLocation(program, "PVM");
	ModMatLoc = glGetUniformLocation(program, "modelMat");
	ViewMatLoc = glGetUniformLocation(program, "viewMat");
	NormMatLoc = glGetUniformLocation(program, "normMat");

	// material
	ambtLoc = glGetUniformLocation(program, "material.ambient");
	diffLoc = glGetUniformLocation(program, "material.diffuse");
	specLoc = glGetUniformLocation(program, "material.specular");
	shinLoc = glGetUniformLocation(program, "material.shininess");
	isLightLoc = glGetUniformLocation(program, "material.isLight");
	// tex
	texSampLoc = glGetUniformLocation(program, "textureSampler");
	CHECK_GL_ERROR();

	return true;
}

void Shader::setTransformUniforms(const mat4& modelMatrix, const mat4& viewMatrix, const mat4& projectionMatrix) {
	glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, value_ptr(projectionMatrix * viewMatrix * modelMatrix));
	glUniformMatrix4fv(ViewMatLoc, 1, GL_FALSE, value_ptr(viewMatrix));
	glUniformMatrix4fv(ModMatLoc, 1, GL_FALSE, value_ptr(modelMatrix));
	glUniformMatrix4fv(NormMatLoc, 1, GL_FALSE, value_ptr(transpose(inverse(viewMatrix))));
	CHECK_GL_ERROR();
}

void Shader::setMaterialUniforms(const vec3& ambient, const vec3& diffuse, const vec3& specular, float shiness, GLuint texture, bool isLight) {

	glUniform3fv(ambtLoc, 1, value_ptr(ambient));
	glUniform3fv(diffLoc, 1, value_ptr(diffuse));
	glUniform3fv(specLoc, 1, value_ptr(specular));
	glUniform1f(shinLoc, shiness);
	CHECK_GL_ERROR();
	
	if (texture != 0)
	{
		glUniform1i(texSampLoc, 0); // 0 -> samplerID, for GPU linker
		glActiveTexture(GL_TEXTURE0);            // 0 -> to be bound, for OpenGL BindTexture
		glBindTexture(GL_TEXTURE_2D, texture);
		CHECK_GL_ERROR();
	}
	else 
	{
		pgr::dieWithError("----------Texture set error!----------");
	}
}
