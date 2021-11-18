#include "Mesh.h"

#include "GL/glew.h"
#include "MathGeoLib.h"

#include <assert.h> 

Mesh::Mesh(aiMesh* _mesh)
{
	m_NumVertices = _mesh->mNumVertices;
	m_NumIndices = _mesh->mNumFaces * 3;

	// CREATING THE VBO
	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	unsigned int vertex_size = (sizeof(float) * 3 + sizeof(float) * 2);
	unsigned int buffer_size = vertex_size * m_NumVertices;
	glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);

	unsigned position_size = sizeof(float) * 3 * m_NumVertices;
	glBufferSubData(GL_ARRAY_BUFFER, 0, position_size, _mesh->mVertices);

	unsigned uv_offset = position_size;
	unsigned uv_size = sizeof(float) * 2 * m_NumVertices;
	float2* uvs = (float2*)(glMapBufferRange(GL_ARRAY_BUFFER, uv_offset, uv_size, GL_MAP_WRITE_BIT));

	for (unsigned int i = 0; i < m_NumVertices; ++i)
	{
		uvs[i] = float2(_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	//CREATE THE EBO
	glGenBuffers(1, &m_Ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);

	unsigned index_size = sizeof(unsigned int) * m_NumIndices;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
	unsigned int* indices = (unsigned int*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_MAP_WRITE_BIT));

	for (unsigned int i = 0; i < _mesh->mNumFaces; ++i)
	{
		assert(_mesh->mFaces[i].mNumIndices == 3); // note: assume triangles = 3 indices per face
		*(indices++) = _mesh->mFaces[i].mIndices[0];
		*(indices++) = _mesh->mFaces[i].mIndices[1];
		*(indices++) = _mesh->mFaces[i].mIndices[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);


	//CREATE THE VAO
	glGenVertexArrays(1, &m_Vao);

	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * m_NumVertices));
}

Mesh::~Mesh()
{
}

void Mesh::Draw()
{
	glBindVertexArray(m_Vao);
	glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, nullptr);
}
