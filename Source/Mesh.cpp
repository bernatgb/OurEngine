#include "Mesh.h"

#include "GL/glew.h"
#include "MathGeoLib.h"

#include <assert.h> 

Mesh::Mesh(aiMesh* _mesh)
{
	//TODO: pass to interleaved

	m_NumVertices = _mesh->mNumVertices;
	m_NumIndices = _mesh->mNumFaces * 3;
	m_MaterialIndex = _mesh->mMaterialIndex;

	// CREATING THE VBO
	MY_LOG("Assimp mesh: Creating the vbo");
	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	unsigned int vertex_size = (sizeof(float) * 3 + sizeof(float) * 2);
	unsigned int buffer_size = vertex_size * m_NumVertices;
	glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);

	float* pointer = (float*)(glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, GL_MAP_WRITE_BIT));

	for (unsigned int i = 0; i < m_NumVertices; ++i)
	{
		*(pointer++) = _mesh->mVertices[i].x;
		*(pointer++) = _mesh->mVertices[i].y;
		*(pointer++) = _mesh->mVertices[i].z;

		*(pointer++) = _mesh->mTextureCoords[0][i].x;
		*(pointer++) = _mesh->mTextureCoords[0][i].y;
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
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 3));
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ebo);
	glDeleteBuffers(1, &m_Vao);
}

void Mesh::Draw() const
{
	glBindVertexArray(m_Vao);
	glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, nullptr);
}
