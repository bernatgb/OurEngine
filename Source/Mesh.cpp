#include "Mesh.h"

#include "GL/glew.h"
#include "MathGeoLib.h"

#include "Application.h"
#include "ModuleDebugDraw.h"

#include "SceneImporter.h"
#include "MeshImporter.h"

#include <assert.h> 

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"



Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ebo);
	glDeleteVertexArrays(1, &m_Vao);

	delete[] m_BB;
}

void Mesh::Draw() const
{
	glBindVertexArray(m_Vao);
	glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

unsigned int* Mesh::MapIndicesBuffer() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);

	return (unsigned*)(glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * m_NumIndices, GL_MAP_WRITE_BIT));
}

float* Mesh::MapVerticesBuffer() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	unsigned int vertex_size = sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3;

	return (float*)(glMapBufferRange(GL_ARRAY_BUFFER, 0, vertex_size * m_NumVertices, GL_MAP_WRITE_BIT));;
}

void Mesh::UnMapIndicesBuffer() const
{
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::UnMapVerticesBuffer() const
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::DrawImGui()
{
	ImGui::Text("Material index: %i", m_MaterialIndex);
	ImGui::Text("Num vertices: %i", m_NumVertices);
	ImGui::Text("Num indices: %i", m_NumIndices);

	ImGui::Text("Min point (local): %f %f %f", m_Min.x, m_Min.y, m_Min.z);
	ImGui::Text("Max point (local): %f %f %f", m_Max.x, m_Max.y, m_Max.z);
}
