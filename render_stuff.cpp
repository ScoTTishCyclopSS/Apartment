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
	uvLoc = 2;
	tnLoc = 3;
	btnLoc = 4;
	nextPosLoc = 5;

	// get uniforms locations
	ModMatLoc = glGetUniformLocation(program, "modelMat");
	ViewMatLoc = glGetUniformLocation(program, "viewMat");
	NormMatLoc = glGetUniformLocation(program, "normMat");
	ProjMatLoc = glGetUniformLocation(program, "projMat");

	// textures
	texSampLoc = glGetUniformLocation(program, "textureSampler");
	normSampLoc = glGetUniformLocation(program, "normalSampler");
	roughSampLoc = glGetUniformLocation(program, "roughSampler");

	// light
	viewPosLoc = glGetUniformLocation(program, "viewPos");

	// time
	tPos = glGetUniformLocation(program, "t");

	CHECK_GL_ERROR();

	return true;
}

void Shader::setPointLightUniforms(int count, std::vector<PointLight *> pointLights) {

	for (int i = 0; i < count; i++)
	{
		char buf[255];

		// vectors
		snprintf(buf, 255, "pointLights[%i].diffuse", i);
		GLuint plightDifPos = glGetUniformLocation(program, buf);

		snprintf(buf, 255, "pointLights[%i].ambient", i);
		GLuint plightAmbPos = glGetUniformLocation(program, buf);

		snprintf(buf, 255, "pointLights[%i].specular", i);
		GLuint plightSpecPos = glGetUniformLocation(program, buf);

		// floats
		snprintf(buf, 255, "pointLights[%i].constant", i);
		GLuint plightConsPos = glGetUniformLocation(program, buf);

		snprintf(buf, 255, "pointLights[%i].linear", i);
		GLuint plightLinPos = glGetUniformLocation(program, buf);

		snprintf(buf, 255, "pointLights[%i].quadratic", i);
		GLuint plightQuadPos = glGetUniformLocation(program, buf);

		// others
		snprintf(buf, 255, "pointLights[%i].blinn", i);
		GLuint plightBlinPos = glGetUniformLocation(program, buf);

		snprintf(buf, 255, "pointLights[%i].position", i);
		GLuint plightPosPos = glGetUniformLocation(program, buf);


		glUniform3fv(plightDifPos, 1, value_ptr(pointLights[i]->diffuse));
		glUniform3fv(plightAmbPos, 1, value_ptr(pointLights[i]->diffuse));
		glUniform3fv(plightSpecPos, 1, value_ptr(pointLights[i]->diffuse));

		glUniform1f(plightConsPos, pointLights[i]->constant);
		glUniform1f(plightLinPos, pointLights[i]->linear);
		glUniform1f(plightQuadPos, pointLights[i]->quadratic);

		glUniform1i(plightBlinPos, pointLights[i]->blinn);
		glUniform3fv(plightPosPos, 1, value_ptr(pointLights[i]->position));
	}

	CHECK_GL_ERROR();
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

void Shader::setMaterialUniforms(GLuint texture, GLuint normal) {
	
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