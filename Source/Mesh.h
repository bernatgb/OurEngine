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
	void DrawBB() const;

	unsigned int GetMaterialIndex() const {
		return m_MaterialIndex;
	};
	unsigned int GetNumVertices() const {
		return m_NumVertices;
	};
	unsigned int GetNumIndices() const {
		return m_NumIndices;
	};
	float3 GetLocalMin() const {
		return m_Min;
	};
	float3 GetLocalMax() const {
		return m_Max;
	};

	void DrawImGui();
	void PrintBB() const; //TODO: REMOVE

private:
	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ebo;
	unsigned int m_MaterialIndex;
	unsigned int m_NumVertices;
	unsigned int m_NumIndices;

	//unsigned int m_VaoBB;
	unsigned int m_VboBB;
	float3* m_BB = new float3[8];

	float3 m_Min;
	float3 m_Max;
};

