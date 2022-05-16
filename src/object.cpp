#include "object.h"

extern int frame;
extern int nextFrame;
extern float t;
extern bool s;

bool Object::loadMesh(const std::string& fileName, Shader shader) {

	int maxBones = 50, maxBonesInfluence = 4;

	Assimp::Importer importer;

	// Unitize object in size (scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// Load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices
		//| aiProcess_FlipUVs
		| aiProcess_CalcTangentSpace
	);

	// abort if the loader fails
	if (scn == NULL || scn->mNumMeshes != 1) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		return false;
	}

	const aiMesh* ai_mesh = scn->mMeshes[0];

	countAttribsPerVertex = 3 + 3 + 2 + 3 + 3; // pos + norm + uv + tn + bitn

	// vertex buffer object, store all vertex positions and normals
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

#pragma region Basic Properties

	// allocate memory for vertices, normals, and texture coordinates
	glBufferData(GL_ARRAY_BUFFER, countAttribsPerVertex * sizeof(float) * ai_mesh->mNumVertices, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * ai_mesh->mNumVertices, ai_mesh->mVertices);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * ai_mesh->mNumVertices, 3 * sizeof(float) * ai_mesh->mNumVertices, ai_mesh->mNormals);

	// just texture 0 for now
	float* textureCoords = new float[2 * ai_mesh->mNumVertices];  // 2 floats per vertex
	float* currentTextureCoord = textureCoords;
	aiVector3D vect;

	// just tangent 0 for now
	float* tangentCoords = new float[3 * ai_mesh->mNumVertices];  // 3 floats per vertex
	float* currTangentCoord = tangentCoords;

	// just tangent 0 for now
	float* bitangentCoords = new float[3 * ai_mesh->mNumVertices];  // 3 floats per vertex
	float* currBitangentCoord = bitangentCoords;

	if (ai_mesh->HasTextureCoords(0)) {

		for (unsigned int idx = 0; idx < ai_mesh->mNumVertices; idx++) {
			vect = (ai_mesh->mTextureCoords[0])[idx];
			*currentTextureCoord++ = vect.x;
			*currentTextureCoord++ = vect.y;
		}
	}

	if (ai_mesh->HasTangentsAndBitangents()) {

		for (unsigned int idx = 0; idx < ai_mesh->mNumVertices; idx++) {
			*currTangentCoord++ = ai_mesh->mTangents[idx].x;
			*currTangentCoord++ = ai_mesh->mTangents[idx].y;
			*currTangentCoord++ = ai_mesh->mTangents[idx].z;
		}

		for (unsigned int idx = 0; idx < ai_mesh->mNumVertices; idx++) {
			*currBitangentCoord++ = ai_mesh->mBitangents[idx].x;
			*currBitangentCoord++ = ai_mesh->mBitangents[idx].y;
			*currBitangentCoord++ = ai_mesh->mBitangents[idx].z;
		}
	}

	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * ai_mesh->mNumVertices, 2 * sizeof(float) * ai_mesh->mNumVertices, textureCoords);
	glBufferSubData(GL_ARRAY_BUFFER, 8 * sizeof(float) * ai_mesh->mNumVertices, 3 * sizeof(float) * ai_mesh->mNumVertices, tangentCoords);
	glBufferSubData(GL_ARRAY_BUFFER, 11 * sizeof(float) * ai_mesh->mNumVertices, 3 * sizeof(float) * ai_mesh->mNumVertices, bitangentCoords);
	
	delete[] bitangentCoords, tangentCoords, textureCoords;

#pragma endregion


#pragma region VBO

	// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
	unsigned int* indices = new unsigned int[ai_mesh->mNumFaces * 3];
	for (unsigned int f = 0; f < ai_mesh->mNumFaces; ++f) {
		indices[f * 3 + 0] = ai_mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = ai_mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = ai_mesh->mFaces[f].mIndices[2];
	}



	// copy our temporary index array to OpenGL and free the array
	glGenBuffers(1, &mesh.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * ai_mesh->mNumFaces, indices, GL_STATIC_DRAW);

	delete[] indices;

	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo); // bind our element array buffer (indices) to vao
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	glEnableVertexAttribArray(shader.posLoc);
	glVertexAttribPointer(shader.posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shader.nrmLoc);
	glVertexAttribPointer(shader.nrmLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * ai_mesh->mNumVertices));
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shader.uvLoc);
	glVertexAttribPointer(shader.uvLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * ai_mesh->mNumVertices));
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shader.tnLoc);
	glVertexAttribPointer(shader.tnLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(8 * sizeof(float) * ai_mesh->mNumVertices));
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shader.btnLoc);
	glVertexAttribPointer(shader.btnLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(11 * sizeof(float) * ai_mesh->mNumVertices));
	CHECK_GL_ERROR();

	glBindVertexArray(0);

#pragma endregion

	countTriangles = ai_mesh->mNumFaces;

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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shader.posLoc);
	glVertexAttribPointer(shader.posLoc, 3, GL_FLOAT, GL_FALSE, countAttribsPerVertex * sizeof(float), (void*)(0)); // (x, y, z) ... ...
	CHECK_GL_ERROR();
	
	glEnableVertexAttribArray(shader.nrmLoc);
	glVertexAttribPointer(shader.nrmLoc, 3, GL_FLOAT, GL_FALSE, countAttribsPerVertex * sizeof(float), (void*)(3 * sizeof(float))); // ... (nx, ny, nz) ...
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shader.uvLoc);
	glVertexAttribPointer(shader.uvLoc, 2, GL_FLOAT, GL_FALSE, countAttribsPerVertex * sizeof(float), (void*)(6 * sizeof(float))); // ... ... (u, v)
	CHECK_GL_ERROR();

	glBindVertexArray(0);

	return;
}

void Object::drawObject(Shader shader, const glm::mat4& viewMat, const glm::mat4& projectMat, const glm::mat4& modelMat) {

	// send mat to shaders
	shader.setTransformUniforms(modelMat, viewMat, projectMat);

	shader.setMaterialUniforms(mesh.texture, mesh.normal, mesh.rough);

	// draw
	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, countTriangles * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	CHECK_GL_ERROR();

	return;
}
