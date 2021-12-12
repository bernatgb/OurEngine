#include "Mesh.h"

#include "GL/glew.h"
#include "MathGeoLib.h"

#include <assert.h> 

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

Mesh::Mesh(aiMesh* _mesh)
{
	m_NumVertices = _mesh->mNumVertices;
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

		if (_mesh->mVertices[i].y > m_Max.y) m_Max.y= _mesh->mVertices[i].y;
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
	
	//CREATING THE BB VBO
	MY_LOG("Assimp mesh: Creating the BB vbo");
	GLfloat BBvertex[] = {
		m_Min.x, m_Max.y, m_Max.z,
		m_Max.x, m_Max.y, m_Max.z,
		m_Min.x, m_Min.y, m_Max.z,
		m_Max.x, m_Min.y, m_Max.z,
		m_Max.x, m_Min.y, m_Min.z,
		m_Max.x, m_Max.y, m_Max.z,
		m_Max.x, m_Max.y, m_Min.z,
		m_Min.x, m_Max.y, m_Max.z,
		m_Min.x, m_Max.y, m_Min.z,
		m_Min.x, m_Min.y, m_Max.z,
		m_Min.x, m_Min.y, m_Min.z,
		m_Max.x, m_Min.y, m_Min.z,
		m_Min.x, m_Max.y, m_Min.z,
		m_Max.x, m_Max.y, m_Min.z,
	};

	glGenBuffers(1, &m_VboBB);
	glBindBuffer(GL_ARRAY_BUFFER, m_VboBB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BBvertex), BBvertex, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	MY_LOG("Assimp mesh: Create correctly");
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ebo);
	glDeleteVertexArrays(1, &m_Vao);

	glDeleteBuffers(1, &m_VboBB);
}

void Mesh::Draw() const
{
	glBindVertexArray(m_Vao);
	glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Mesh::DrawBB() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);

	/*// TEST
	GLfloat BBvertex[] = {
		m_Min.x, m_Max.y, m_Max.z,
		m_Max.x, m_Max.y, m_Max.z,
		m_Min.x, m_Min.y, m_Max.z,
		m_Max.x, m_Min.y, m_Max.z,
		m_Max.x, m_Min.y, m_Min.z,
		m_Max.x, m_Max.y, m_Max.z,
		m_Max.x, m_Max.y, m_Min.z,
		m_Min.x, m_Max.y, m_Max.z,
		m_Min.x, m_Max.y, m_Min.z,
		m_Min.x, m_Min.y, m_Max.z,
		m_Min.x, m_Min.y, m_Min.z,
		m_Max.x, m_Min.y, m_Min.z,
		m_Min.x, m_Max.y, m_Min.z,
		m_Max.x, m_Max.y, m_Min.z,
	};

	unsigned int aux;
	glGenBuffers(1, &aux);
	glBindBuffer(GL_ARRAY_BUFFER, aux);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BBvertex), BBvertex, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//

	glBindBuffer(GL_ARRAY_BUFFER, aux);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &aux);*/

	glBindBuffer(GL_ARRAY_BUFFER, m_VboBB);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Mesh::DrawImGui()
{
	ImGui::Text("Material index: %i", m_MaterialIndex);
	ImGui::Text("Num vertices: %i", m_NumVertices);
	ImGui::Text("Num indices: %i", m_NumIndices);

	ImGui::Text("Min point: %f %f %f", m_Min.x, m_Min.y, m_Min.z);
	ImGui::Text("Max point: %f %f %f", m_Max.x, m_Max.y, m_Max.z);
}
