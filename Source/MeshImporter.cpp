#include "MeshImporter.h"

#include "GL/glew.h"
#include "Math/float3.h"

void importer::mesh::Import(const aiMesh* mesh, Mesh* ourMesh)
{
	ourMesh->m_GUID = rand();
	ourMesh->m_NumVertices = mesh->mNumVertices;
	ourMesh->m_NumIndices = mesh->mNumFaces * 3;
	ourMesh->m_MaterialIndex = mesh->mMaterialIndex;

	// CREATING THE VBO
	MY_LOG("Assimp mesh: Creating the vbo");
	glGenBuffers(1, &ourMesh->m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ourMesh->m_Vbo);

	unsigned int vertex_size = sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3;
	unsigned int buffer_size = vertex_size * ourMesh->m_NumVertices;
	glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);

	float* pointer = (float*)(glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, GL_MAP_WRITE_BIT));

	ourMesh->m_Min = float3(mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z);
	ourMesh->m_Max = float3(mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z);

	for (unsigned int i = 0; i < ourMesh->m_NumVertices; ++i)
	{
		if (mesh->mVertices[i].x > ourMesh->m_Max.x) ourMesh->m_Max.x = mesh->mVertices[i].x;
		if (mesh->mVertices[i].x < ourMesh->m_Min.x) ourMesh->m_Min.x = mesh->mVertices[i].x;

		if (mesh->mVertices[i].y > ourMesh->m_Max.y) ourMesh->m_Max.y = mesh->mVertices[i].y;
		if (mesh->mVertices[i].y < ourMesh->m_Min.y) ourMesh->m_Min.y = mesh->mVertices[i].y;
		
		if (mesh->mVertices[i].z > ourMesh->m_Max.z) ourMesh->m_Max.z = mesh->mVertices[i].z;
		if (mesh->mVertices[i].z < ourMesh->m_Min.z) ourMesh->m_Min.z = mesh->mVertices[i].z;

		*(pointer++) = mesh->mVertices[i].x;
		*(pointer++) = mesh->mVertices[i].y;
		*(pointer++) = mesh->mVertices[i].z;

		*(pointer++) = mesh->mTextureCoords[0][i].x;
		*(pointer++) = mesh->mTextureCoords[0][i].y;

		*(pointer++) = mesh->mNormals[i].x;
		*(pointer++) = mesh->mNormals[i].y;
		*(pointer++) = mesh->mNormals[i].z;

		//*(pointer++) = mesh->mTangents[i].x;
		//*(pointer++) = mesh->mTangents[i].y;
		//*(pointer++) = mesh->mTangents[i].z;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	//CREATE THE EBO
	MY_LOG("Assimp mesh: Creating the ebo");
	glGenBuffers(1, &ourMesh->m_Ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourMesh->m_Ebo);

	unsigned index_size = sizeof(unsigned int) * ourMesh->m_NumIndices;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
	unsigned* indices = (unsigned*)(glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, index_size, GL_MAP_WRITE_BIT));
		
	ourMesh->m_Triangles = std::vector<Triangle>(ourMesh->m_NumIndices / 3);

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3); // note: assume triangles = 3 indices per face
		*(indices++) = mesh->mFaces[i].mIndices[0];
		*(indices++) = mesh->mFaces[i].mIndices[1];
		*(indices++) = mesh->mFaces[i].mIndices[2];

		//CREATING TRIANGLES VECTOR
		ourMesh->m_Triangles[i] = Triangle(
			float3(mesh->mVertices[mesh->mFaces[i].mIndices[0]].x, mesh->mVertices[mesh->mFaces[i].mIndices[0]].y, mesh->mVertices[mesh->mFaces[i].mIndices[0]].z),
			float3(mesh->mVertices[mesh->mFaces[i].mIndices[1]].x, mesh->mVertices[mesh->mFaces[i].mIndices[1]].y, mesh->mVertices[mesh->mFaces[i].mIndices[1]].z),
			float3(mesh->mVertices[mesh->mFaces[i].mIndices[2]].x, mesh->mVertices[mesh->mFaces[i].mIndices[2]].y, mesh->mVertices[mesh->mFaces[i].mIndices[2]].z)
		);
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	//CREATE THE VAO
	MY_LOG("Assimp mesh: Creating the vao");
	glGenVertexArrays(1, &ourMesh->m_Vao);

	glBindVertexArray(ourMesh->m_Vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourMesh->m_Ebo);
	glBindBuffer(GL_ARRAY_BUFFER, ourMesh->m_Vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 3 + sizeof(float) * 2));

	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 3 + sizeof(float) * 2));

	glBindVertexArray(0);
	
	//CREATING THE BB
	ourMesh->m_BB = new float3[8];
	ourMesh->m_BB[0] = float3(ourMesh->m_Min.x, ourMesh->m_Max.y, ourMesh->m_Max.z);
	ourMesh->m_BB[1] = float3(ourMesh->m_Min.x, ourMesh->m_Max.y, ourMesh->m_Min.z);
	ourMesh->m_BB[2] = float3(ourMesh->m_Max.x, ourMesh->m_Max.y, ourMesh->m_Min.z);
	ourMesh->m_BB[3] = float3(ourMesh->m_Max.x, ourMesh->m_Max.y, ourMesh->m_Max.z);
	ourMesh->m_BB[4] = float3(ourMesh->m_Min.x, ourMesh->m_Min.y, ourMesh->m_Max.z);
	ourMesh->m_BB[5] = float3(ourMesh->m_Min.x, ourMesh->m_Min.y, ourMesh->m_Min.z);
	ourMesh->m_BB[6] = float3(ourMesh->m_Max.x, ourMesh->m_Min.y, ourMesh->m_Min.z);
	ourMesh->m_BB[7] = float3(ourMesh->m_Max.x, ourMesh->m_Min.y, ourMesh->m_Max.z);

	MY_LOG("Assimp mesh: Create correctly");
}

int importer::mesh::Save(const Mesh* ourMesh, char*& fileBuffer)
{
	// MAYBE USE AN EXTRA BYTE TO STORE MESH INFORMATION (IF THE MESH DOESN'T HAVE TEXCOORDS/NORMALS/...)
	// 0 - indices
	// 1 - texCoords
	// 2 - normales
	//byte info = 0;
	
	unsigned int header[4] = {
		ourMesh->m_GUID,
		ourMesh->m_NumVertices,
		ourMesh->m_NumIndices,
		ourMesh->m_MaterialIndex
	};

	float extremes[6] = {
		ourMesh->m_Min.x, ourMesh->m_Min.y, ourMesh->m_Min.z,
		ourMesh->m_Max.x, ourMesh->m_Max.y, ourMesh->m_Max.z
	};

	unsigned int size = sizeof(header) + sizeof(extremes) + sizeof(unsigned int) * ourMesh->m_NumIndices + ourMesh->m_NumVertices * sizeof(float) * 8 + ourMesh->m_Triangles.size() * sizeof(Triangle);

	// Allocate
	fileBuffer = new char[size];
	char* cursor = fileBuffer;

	// Store info
	//unsigned int bytes = sizeof(byte);
	//memcpy(cursor, &info, sizeof(byte));
	//cursor += bytes;

	// Store header
	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, sizeof(header));
	cursor += bytes;

	// Store extremes
	bytes = sizeof(extremes);
	memcpy(cursor, extremes, sizeof(extremes));
	cursor += bytes;

	// Store vertices
	bytes = sizeof(float) * 8 * ourMesh->m_NumVertices;
	memcpy(cursor, ourMesh->MapVerticesBuffer(), bytes);
	ourMesh->UnMapVerticesBuffer();
	cursor += bytes;
	
	// Store indices
	bytes = sizeof(unsigned int) * ourMesh->m_NumIndices;
	memcpy(cursor, ourMesh->MapIndicesBuffer(), bytes);
	ourMesh->UnMapIndicesBuffer();
	cursor += bytes;

	// Store triangles
	bytes = sizeof(Triangle) * ourMesh->m_Triangles.size();
	memcpy(cursor, &ourMesh->m_Triangles[0], bytes);
	cursor += bytes;

	return size;
}

void importer::mesh::Load(const char* fileBuffer, Mesh* ourMesh)
{
	const char* cursor = fileBuffer;

	MY_LOG("MeshImporter_Load: Reading main variables");
	unsigned int header[4];
	unsigned int bytes = sizeof(header);
	memcpy(header, cursor, bytes);
	cursor += bytes;

	ourMesh->m_GUID = header[0];
	ourMesh->m_NumVertices = header[1];
	ourMesh->m_NumIndices = header[2];
	ourMesh->m_MaterialIndex = header[3];

	float extremes[6];
	bytes = sizeof(extremes);
	memcpy(extremes, cursor, bytes);
	cursor += bytes;

	ourMesh->m_Min = float3(extremes[0], extremes[1], extremes[2]);
	ourMesh->m_Max = float3(extremes[3], extremes[4], extremes[5]);

	// CREATING THE VBO
	MY_LOG("MeshImporter_Load: Reading vertices and creating vbo");
	glGenBuffers(1, &ourMesh->m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ourMesh->m_Vbo);

	unsigned int vertex_size = sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3;
	unsigned int buffer_size = vertex_size * ourMesh->m_NumVertices;
	glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);

	float* pointer = (float*)(glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, GL_MAP_WRITE_BIT));

	bytes = sizeof(float) * 8 * ourMesh->m_NumVertices;
	memcpy(pointer, cursor, bytes);
	cursor += bytes;

	glUnmapBuffer(GL_ARRAY_BUFFER);

	//CREATE THE EBO
	MY_LOG("MeshImporter_Load: Reading indices and creating ebo");
	glGenBuffers(1, &ourMesh->m_Ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourMesh->m_Ebo);

	unsigned index_size = sizeof(unsigned int) * ourMesh->m_NumIndices;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
	unsigned* indices = (unsigned*)(glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, index_size, GL_MAP_WRITE_BIT));

	bytes = sizeof(unsigned int) * ourMesh->m_NumIndices;
	memcpy(indices, cursor, bytes);
	cursor += bytes;

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	//CREATE THE VAO
	MY_LOG("MeshImporter_Load: Creating the vao");
	glGenVertexArrays(1, &ourMesh->m_Vao);

	glBindVertexArray(ourMesh->m_Vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourMesh->m_Ebo);
	glBindBuffer(GL_ARRAY_BUFFER, ourMesh->m_Vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 3 + sizeof(float) * 2));

	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 3 + sizeof(float) * 2));

	glBindVertexArray(0);

	//CREATING THE BB
	ourMesh->m_BB = new float3[8];
	ourMesh->m_BB[0] = float3(ourMesh->m_Min.x, ourMesh->m_Max.y, ourMesh->m_Max.z);
	ourMesh->m_BB[1] = float3(ourMesh->m_Min.x, ourMesh->m_Max.y, ourMesh->m_Min.z);
	ourMesh->m_BB[2] = float3(ourMesh->m_Max.x, ourMesh->m_Max.y, ourMesh->m_Min.z);
	ourMesh->m_BB[3] = float3(ourMesh->m_Max.x, ourMesh->m_Max.y, ourMesh->m_Max.z);
	ourMesh->m_BB[4] = float3(ourMesh->m_Min.x, ourMesh->m_Min.y, ourMesh->m_Max.z);
	ourMesh->m_BB[5] = float3(ourMesh->m_Min.x, ourMesh->m_Min.y, ourMesh->m_Min.z);
	ourMesh->m_BB[6] = float3(ourMesh->m_Max.x, ourMesh->m_Min.y, ourMesh->m_Min.z);
	ourMesh->m_BB[7] = float3(ourMesh->m_Max.x, ourMesh->m_Min.y, ourMesh->m_Max.z);

	//RESTORING TRIANGLES VECTOR
	bytes = sizeof(Triangle) * ourMesh->m_NumIndices / 3;
	ourMesh->m_Triangles = std::vector<Triangle>(ourMesh->m_NumIndices / 3);
	memcpy(&ourMesh->m_Triangles[0], cursor, bytes);
	cursor += bytes;

	MY_LOG("MeshImporter_Load: Loading complete");
}
