#include "render_stuff.h"

bool Shader::init(std::string vert, std::string frag) {
	std::vector<GLuint> shaders;

	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, vert));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, frag));

	if (shaders[0] == 0 || shaders[1] == 0) {
		std::cout << "---------- Shaders creation error! ----------\n";
		return false;
	}
		
	program = pgr::createProgram(shaders); // create program with 2 shaders
	if (program == 0) {
		std::cout << "---------- Shaders program creation error! ----------\n";
		return false;
	}

	// get vertex attributes locations
	posLoc = 0;
	nrmLoc = 1;
	texLoc = 2;
	tnLoc = 3;
	btnLoc = 4;

	// get uniforms locations
	ModMatLoc = glGetUniformLocation(program, "modelMat");
	ViewMatLoc = glGetUniformLocation(program, "viewMat");
	NormMatLoc = glGetUniformLocation(program, "normMat");
	ProjMatLoc = glGetUniformLocation(program, "projMat");

	// lighting
	lightPosLoc = glGetUniformLocation(program, "lightPos");
	viewPosLoc = glGetUniformLocation(program, "viewPos");

	// material
	ambtLoc = glGetUniformLocation(program, "material.ambient");
	diffLoc = glGetUniformLocation(program, "material.diffuse");
	specLoc = glGetUniformLocation(program, "material.specular");
	shinLoc = glGetUniformLocation(program, "material.shininess");
	// tex
	texSampLoc = glGetUniformLocation(program, "textureSampler");
	normSampLoc = glGetUniformLocation(program, "normalSampler");
	CHECK_GL_ERROR();

	return true;
}

void Shader::setTransformUniforms(const mat4& modelMatrix, const mat4& viewMatrix, const mat4& projectionMatrix) {

	glUniformMatrix4fv(ProjMatLoc, 1, GL_FALSE, value_ptr(projectionMatrix));
	CHECK_GL_ERROR();
	glUniformMatrix4fv(ViewMatLoc, 1, GL_FALSE, value_ptr(viewMatrix));
	CHECK_GL_ERROR();
	glUniformMatrix4fv(ModMatLoc, 1, GL_FALSE, value_ptr(modelMatrix));
	CHECK_GL_ERROR();
	glUniformMatrix4fv(NormMatLoc, 1, GL_FALSE, value_ptr(transpose(inverse(viewMatrix))));
	CHECK_GL_ERROR();
}

void Shader::setMaterialUniforms(const vec3& ambient, const vec3& diffuse, const vec3& specular, float shiness, GLuint texture, GLuint normal, bool isLight) {

	glUniform3fv(ambtLoc, 1, value_ptr(ambient));
	CHECK_GL_ERROR();
	glUniform3fv(diffLoc, 1, value_ptr(diffuse));
	CHECK_GL_ERROR();
	glUniform3fv(specLoc, 1, value_ptr(specular));
	CHECK_GL_ERROR();
	glUniform1f(shinLoc, shiness);
	CHECK_GL_ERROR();
	
	if (texture != 0)
	{
		glUniform1i(texSampLoc, 0); // 0 -> samplerID, for GPU linker
		glActiveTexture(GL_TEXTURE0);            // 0 -> to be bound, for OpenGL BindTexture
		glBindTexture(GL_TEXTURE_2D, texture);
		CHECK_GL_ERROR();
	}

	if (normal != 0)
	{
		glUniform1i(normSampLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal); // todo -> check 0
		CHECK_GL_ERROR();
	}
}


GLuint loadCubemap(std::vector<std::string> faces)
{
	GLuint textureID;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (int i = 0; i < faces.size(); i++)
	{
		std::cout << "loading " << faces[i] << std::endl;
		if (!pgr::loadTexImage2D(faces[i], GL_TEXTURE_CUBE_MAP_POSITIVE_X + i))
			pgr::dieWithError("------------ Cannot load cubemap texture! ------------");
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}
