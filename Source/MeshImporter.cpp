#include "MeshImporter.h"

#include "GL/glew.h"
#include "Math/float3.h"

void importer::mesh::Import(const aiMesh* mesh, Mesh* ourMesh)
{
	/*m_NumVertices = _mesh->mNumVertices;
	m_NumIndices = _mesh->mNumFaces * 3;
	m_MaterialIndex = _mesh->mMaterialIndex;

	// CREATING THE VBO
	MY_LOG("Assimp mesh: Creating the vbo");
	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	unsigned int vertex_size = sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3;
	unsigned int buffer_size = vertex_size * m_NumVertices;
	glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);

	float* pointer = (float*)(glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, GL_MAP_WRITE_BIT));

	m_Min = float3(_mesh->mVertices[0].x, _mesh->mVertices[0].y, _mesh->mVertices[0].z);
	m_Max = float3(_mesh->mVertices[0].x, _mesh->mVertices[0].y, _mesh->mVertices[0].z);

	for (unsigned int i = 0; i < m_NumVertices; ++i)
	{
		if (_mesh->mVertices[i].x > m_Max.x) m_Max.x = _mesh->mVertices[i].x;
		if (_mesh->mVertices[i].x < m_Min.x) m_Min.x = _mesh->mVertices[i].x;

		if (_mesh->mVertices[i].y > m_Max.y) m_Max.y = _mesh->mVertices[i].y;
		if (_mesh->mVertices[i].y < m_Min.y) m_Min.y = _mesh->mVertices[i].y;

		if (_mesh->mVertices[i].z > m_Max.z) m_Max.z = _mesh->mVertices[i].z;
		if (_mesh->mVertices[i].z < m_Min.z) m_Min.z = _mesh->mVertices[i].z;

		*(pointer++) = _mesh->mVertices[i].x;
		*(pointer++) = _mesh->mVertices[i].y;
		*(pointer++) = _mesh->mVertices[i].z;

		*(pointer++) = _mesh->mTextureCoords[0][i].x;
		*(pointer++) = _mesh->mTextureCoords[0][i].y;

		*(pointer++) = _mesh->mNormals[i].x;
		*(pointer++) = _mesh->mNormals[i].y;
		*(pointer++) = _mesh->mNormals[i].z;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	//CREATE THE EBO
	MY_LOG("Assimp mesh: Creating the ebo");
	glGenBuffers(1, &m_Ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);

	unsigned index_size = sizeof(unsigned int) * m_NumIndices;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
	unsigned* indices = (unsigned*)(glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, index_size, GL_MAP_WRITE_BIT));

	for (unsigned int i = 0; i < _mesh->mNumFaces; ++i)
	{
		assert(_mesh->mFaces[i].mNumIndices == 3); // note: assume triangles = 3 indices per face
		*(indices++) = _mesh->mFaces[i].mIndices[0];
		*(indices++) = _mesh->mFaces[i].mIndices[1];
		*(indices++) = _mesh->mFaces[i].mIndices[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	//CREATE THE VAO
	MY_LOG("Assimp mesh: Creating the vao");
	glGenVertexArrays(1, &m_Vao);

	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 3 + sizeof(float) * 2));

	glBindVertexArray(0);
	*/
	MY_LOG("Assimp mesh: Create correctly");
}

void importer::mesh::Save(const Mesh* ourMesh, char*& fileBuffer)
{
	// MAYBE USE AN EXTRA BYTE TO STORE MESH INFORMATION (IF THE MESH DOESN'T HAVE TEXCOORDS/NORMALS/...)
	// 0 - indices
	// 1 - texCoords
	// 2 - normales
	//byte info = 0;

	unsigned int guid = 4;

	unsigned int header[4] = {
		guid,
		ourMesh->m_NumVertices,
		ourMesh->m_NumIndices,
		ourMesh->m_MaterialIndex //??
	};

	float extremes[6] = {
		ourMesh->m_Min.x, ourMesh->m_Min.y, ourMesh->m_Min.z,
		ourMesh->m_Max.x, ourMesh->m_Max.y, ourMesh->m_Max.z
	};

	unsigned int size = sizeof(header) + sizeof(extremes) + sizeof(unsigned int) * ourMesh->m_NumIndices + ourMesh->m_NumVertices * sizeof(float) * 8;

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
	ourMesh->UnMapBuffer();
	cursor += bytes;
	
	// Store indices
	bytes = sizeof(unsigned int) * ourMesh->m_NumIndices;
	memcpy(cursor, ourMesh->MapIndicesBuffer(), bytes);
	ourMesh->UnMapBuffer();
	cursor += bytes;
}

void importer::mesh::Load(const char* fileBuffer, Mesh* ourMesh)
{
	const char* cursor = fileBuffer;

	MY_LOG("MeshImporter_Load: Reading main variables");
	unsigned int header[4];
	unsigned int bytes = sizeof(header);
	memcpy(header, cursor, bytes);
	cursor += bytes;

	ourMesh->m_NumVertices = header[1];
	ourMesh->m_NumIndices = header[2];
	ourMesh->m_MaterialIndex = header[3];

	float extremes[6];
	bytes = sizeof(extremes);
	memcpy(extremes, cursor, bytes);
	cursor += bytes;

	ourMesh->m_Min = float3(header[0], header[1], header[2]);
	ourMesh->m_Max = float3(header[3], header[4], header[5]);

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

	glBindVertexArray(0);

	MY_LOG("MeshImporter_Load: Loading complete");
}

