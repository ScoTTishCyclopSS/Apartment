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

	// light (& mb fog)
	viewPosLoc = glGetUniformLocation(program, "viewPos");

	// time
	tPos = glGetUniformLocation(program, "t");

	// water
	waveStrengthLoc = glGetUniformLocation(program, "waveStrength");
	layerLoc = glGetUniformLocation(program, "layer");

	CHECK_GL_ERROR();

	return true;
}

void Shader::setFogUniforms(vec3 color, float start, float end, float dens, int type, bool active) {
	GLuint fogColorPos = glGetUniformLocation(program, "fog.color");
	GLuint fogStartPos = glGetUniformLocation(program, "fog.start");
	GLuint fogEndPos = glGetUniformLocation(program, "fog.end");
	GLuint fogDensPos = glGetUniformLocation(program, "fog.density");
	GLuint fogTypePos = glGetUniformLocation(program, "fog.type");
	GLuint fogActivePos = glGetUniformLocation(program, "fog.isActive");

	glUniform3fv(fogColorPos, 1, value_ptr(color));
	glUniform1f(fogStartPos, start);
	glUniform1f(fogEndPos, end);
	glUniform1f(fogDensPos, dens);
	glUniform1i(fogTypePos, type);
	glUniform1i(fogActivePos, active);
}

void Shader::setSpotLightUniforms(SpotLight *spl, bool active) {
	GLuint plightDirPos = glGetUniformLocation(program, "spotLight.direction");
	GLuint plightPosPos = glGetUniformLocation(program, "spotLight.position");
	GLuint plightCutPos = glGetUniformLocation(program, "spotLight.cut");
	GLuint plightDiffPos = glGetUniformLocation(program, "spotLight.diffuse");
	GLuint plightAmbPos = glGetUniformLocation(program, "spotLight.ambient");
	GLuint plightSpecPos = glGetUniformLocation(program, "spotLight.specular");
	GLuint plightSwitchPos = glGetUniformLocation(program, "spotLight.isActive");

	glUniform3fv(plightDirPos, 1, value_ptr(spl->direction));
	glUniform3fv(plightPosPos, 1, value_ptr(spl->position));
	glUniform3fv(plightDiffPos, 1, value_ptr(spl->diffuse));
	glUniform3fv(plightAmbPos, 1, value_ptr(spl->ambient));
	glUniform3fv(plightSpecPos, 1, value_ptr(spl->specular));
	glUniform1f(plightCutPos, spl->cut);
	glUniform1i(plightSwitchPos, active);

	CHECK_GL_ERROR();
}

void Shader::setDirLightUniforms(DirectLight* dl) {
	GLuint plightDirPos = glGetUniformLocation(program, "directLight.direction");
	GLuint plightDiffPos = glGetUniformLocation(program, "directLight.diffuse");
	GLuint plightAmbPos = glGetUniformLocation(program, "directLight.ambient");
	GLuint plightSpecPos = glGetUniformLocation(program, "directLight.specular");

	glUniform3fv(plightDirPos, 1, value_ptr(dl->direction));
	glUniform3fv(plightDiffPos, 1, value_ptr(dl->diffuse));
	glUniform3fv(plightAmbPos, 1, value_ptr(dl->ambient));
	glUniform3fv(plightSpecPos, 1, value_ptr(dl->specular));

	CHECK_GL_ERROR();
}

void Shader::setPointLightUniforms(std::vector<PointLight *> pointLights) {

	for (int i = 0; i < pointLights.size(); i++)
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

void Shader::setMaterialUniforms(GLuint texture, GLuint normal, GLuint rough) {
	
	if (texture != 0)
	{
		glUniform1i(texSampLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		CHECK_GL_ERROR();
	}

	if (normal != 0)
	{
		glUniform1i(normSampLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal);
		CHECK_GL_ERROR();
	}

	if (rough != 0)
	{
		glUniform1i(roughSampLoc, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, rough);
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