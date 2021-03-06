#pragma once
#include "Globals.h"

#include "MathGeoLib.h"

#include "assimp/cimport.h"
#include "Assimp/scene.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	void Draw() const;
	void DrawImGui();

	unsigned int* MapIndicesBuffer() const;
	float* MapVerticesBuffer() const;
	void UnMapIndicesBuffer() const;
	void UnMapVerticesBuffer() const;

	unsigned int m_GUID;

	unsigned int m_MaterialIndex;
	unsigned int m_NumVertices;
	unsigned int m_NumIndices;

	float3 m_Min;
	float3 m_Max;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ebo;

	float3* m_BB = nullptr;

	std::vector<Triangle> m_Triangles;
};
