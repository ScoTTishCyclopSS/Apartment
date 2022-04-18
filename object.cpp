#include "object.h"

void Object::initModel(Shader shader) {

		// VBO
		glGenBuffers(1, &mesh.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * countVertices * countAttribsPerVertex, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERROR();

		// EBO
		glGenBuffers(1, &mesh.ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * countTriangles * 3, triangles, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		CHECK_GL_ERROR();

		// VAO
		glGenVertexArrays(1, &mesh.vao);

		// connect vertex attribute
		glBindVertexArray(mesh.vao);

		// vertex pos
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glEnableVertexAttribArray(shader.posLoc);
		glVertexAttribPointer(shader.posLoc, 3, GL_FLOAT, GL_FALSE, countAttribsPerVertex * sizeof(float), (void*)(0)); // (x, y) ... ...

		// triangle indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);

		//obj->mesh->ambient = glm::vec3(1.0f, 1.0f, 1.0f);
		//obj->mesh->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		//obj->mesh->specular = glm::vec3(1.0f, 1.0f, 1.0f);
		//obj->mesh->shiness = 1.0f;
		//obj->mesh->texture = 0;

		glBindVertexArray(0);
		CHECK_GL_ERROR();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	return;
}

void Object::drawObject(Shader shader, const glm::mat4& viewMat, const glm::mat4& projectMat, const glm::mat4& modelMat) {

	glUniformMatrix4fv(shader.PVMLoc, 1, GL_FALSE, value_ptr(projectMat * viewMat * modelMat));

	glBindVertexArray(mesh.vao);
	CHECK_GL_ERROR();
	glDrawElements(GL_TRIANGLES, countTriangles * 3, GL_UNSIGNED_INT, (void*)0);
	CHECK_GL_ERROR();
	glBindVertexArray(0);
	CHECK_GL_ERROR();
	

	// send mat to shaders
	shader.setTransformUniforms(modelMat, viewMat, projectMat);
	return;
	
	// send material info
	//setMaterialUniforms(
	//	object->mesh->ambient,
	//	object->mesh->diffuse,
	//	object->mesh->specular,
	//	object->mesh->shiness,
	//	object->mesh->texture
	//);
}
