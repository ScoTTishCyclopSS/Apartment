#include "object.h"

bool Object::loadMesh(const std::string& fileName, GLuint positionL, GLuint normalL, GLuint* vbo, GLuint* eao, GLuint* vao, int* numTriangles) {
	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1); // Unitize object in size (scale the model to fit into (-1..1)^3)
	// Load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);
	// abort if the loader fails
	if (!scn) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		return false;
	}
	// some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example
	if (scn->mNumMeshes != 1) {
		std::cerr << "this simplified loader can only process files with only one mesh" << std::endl;
		return false;
	}
	// in this phase we know we have one mesh in our loaded scene, we can directly copy its data to opengl ...
	const aiMesh* mesh = scn->mMeshes[0];

	// vertex buffer object, store all vertex positions and normals
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices * 2, 0, GL_STATIC_DRAW); // allocate memory for vertices and normals
	// first store all vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
	// then all normals
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

	// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
	unsigned* indices = new unsigned[mesh->mNumFaces * 3];
	for (unsigned f = 0; f < mesh->mNumFaces; ++f) {
		indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}
	// copy our temporary index array to opengl and free the array
	glGenBuffers(1, eao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *eao);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);
	delete[] indices;

	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *eao); // bind our element array buffer (indices) to vao
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glEnableVertexAttribArray(positionL);
	glVertexAttribPointer(positionL, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(normalL);
	glVertexAttribPointer(normalL, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));
	glBindVertexArray(0);

	*numTriangles = mesh->mNumFaces;
	return true;
}

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
	glBindVertexArray(mesh.vao);
	CHECK_GL_ERROR();

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shader.posLoc);
	glVertexAttribPointer(shader.posLoc, 3, GL_FLOAT, GL_FALSE, countAttribsPerVertex * sizeof(float), (void*)(0)); // (x, y, z) ... ...
	CHECK_GL_ERROR();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shader.nrmLoc);
	glVertexAttribPointer(shader.nrmLoc, 3, GL_FLOAT, GL_FALSE, countAttribsPerVertex * sizeof(float), (void*)(3 * sizeof(float))); // ... (nx, ny, nz) ...
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shader.texLoc);
	glVertexAttribPointer(shader.texLoc, 2, GL_FLOAT, GL_FALSE, countAttribsPerVertex * sizeof(float), (void*)(6 * sizeof(float))); // ... ... (u, v)
	CHECK_GL_ERROR();

	glBindVertexArray(0);

	return;
}

void Object::drawObject(Shader shader, const glm::mat4& viewMat, const glm::mat4& projectMat, const glm::mat4& modelMat) {

	// send mat to shaders
	shader.setTransformUniforms(
		modelMat, 
		viewMat, 
		projectMat
	);

	// send material info
	shader.setMaterialUniforms(
		material.ambient, 
		material.diffuse,
		material.specular, 
		material.shiness, 
		mesh.texture,
		isLightsource
	);

	// draw
	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, countTriangles * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	CHECK_GL_ERROR();

	return;
}
