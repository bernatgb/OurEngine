#pragma once
#include "Globals.h"

#include "MathGeoLib.h"

#include "assimp/cimport.h"
#include "Assimp/scene.h"

class Mesh
{
public:
	Mesh(aiMesh* _mesh);
	~Mesh();

	void Draw() const;

	unsigned int GetMaterialIndex() const {
		return m_MaterialIndex;
	};
	unsigned int GetNumVertices() const {
		return m_NumVertices;
	};
	unsigned int GetNumIndices() const {
		return m_NumIndices;
	};
	float3 GetMin() const {
		return m_Min;
	};
	float3 GetMax() const {
		return m_Max;
	};

	void DrawImGui();

private:
	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ebo;
	unsigned int m_MaterialIndex;
	unsigned int m_NumVertices;
	unsigned int m_NumIndices;

	float3 m_Min;
	float3 m_Max;
};

