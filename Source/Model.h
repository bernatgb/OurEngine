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
		return m_ModelMatrix.Transposed() * float4(m_Max.x, m_Max.y, m_Max.z, 1.0f);
	};
	float4 GetMinPoint() const {
		return m_ModelMatrix.Transposed() * float4(m_Min.x, m_Min.y, m_Min.z, 1.0f);
	};
	float4 GetCenter() const {
		return m_ModelMatrix.Transposed() * float4(m_Min.x + (m_Max.x-m_Min.x)/2.0f, m_Min.y + (m_Max.y-m_Min.y)/2.0f, m_Min.z + (m_Max.z-m_Min.z)/2.0f, 1.0f);
	};
	float4 GetBase() const {
		return m_ModelMatrix.Transposed() * float4(m_Min.x + (m_Max.x-m_Min.x)/2.0f, m_Min.y, m_Min.z + (m_Max.z-m_Min.z)/2.0f, 1.0f);
	};
	float GetDiameter() const {
		return Distance3(GetMaxPoint(), GetMinPoint());
	};

private:
	void LoadMeshes(aiMesh** _meshes, const unsigned int& _numMeshes);
	void LoadTextures(aiMaterial** _materials, const unsigned int& _numMaterials);

	char* m_Name = nullptr;
	std::vector<Mesh*> m_Meshes;
	std::vector<Texture*> m_Textures;

	float4x4 m_ModelMatrix;
	float3 m_Position, m_RotationEuler, m_Scale;
	Quat m_Rotation;

	unsigned int m_NumVertices = 0;
	unsigned int m_NumTriangles = 0;

	float3 m_Min;
	float3 m_Max;
};

