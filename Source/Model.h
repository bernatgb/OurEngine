#pragma once
#include "Globals.h"

#include "Mesh.h"
#include "Texture.h"

#include "MathGeoLib.h"
#include <vector>

class Model
{
public:
	Model(const char* _fileName);
	~Model();

	void Draw(const unsigned int& _program) const;

	void DrawImGui();

	float4 GetMaxPoint() const {
		return m_ModelMatrix.Transposed() * float4(m_MaxX, m_MaxY, m_MaxZ, 1.0f);
	};
	float4 GetMinPoint() const {
		return m_ModelMatrix.Transposed() * float4(m_MinX, m_MinY, m_MinZ, 1.0f);
	};
	float4 GetCenter() const {
		return m_ModelMatrix.Transposed() * float4(m_MinX + (m_MaxX - m_MinX)/2.0f, m_MinY + (m_MaxY - m_MinY) / 2.0f, m_MinZ + (m_MaxZ - m_MinZ) / 2.0f, 1.0f);
	};
	float4 GetBase() const {
		return m_ModelMatrix.Transposed() * float4(m_MinX + (m_MaxX - m_MinX) / 2.0f, m_MinY, m_MinZ + (m_MaxZ - m_MinZ) / 2.0f, 1.0f);
	};
	float GetRadius() const {
		float4 aux1 = GetMaxPoint();
		float4 aux2 = GetMinPoint();
		float d = Distance3(aux1, aux2);
		return Distance3(GetMaxPoint(), GetMinPoint());
	};

private:
	void LoadMeshes(aiMesh** _meshes, const unsigned int& _numMeshes);
	void LoadTextures(aiMaterial** _materials, const unsigned int& _numMaterials);

	char* m_Name = nullptr;
	std::vector<Mesh*> m_Meshes;
	std::vector<Texture*> m_Textures;

	float4x4 m_ModelMatrix;
	float3 m_Position, m_Rotation, m_Scale;

	unsigned int m_NumVertices = 0;
	unsigned int m_NumTriangles = 0;

	float m_MinX, m_MaxX;
	float m_MinY, m_MaxY;
	float m_MinZ, m_MaxZ;
};

