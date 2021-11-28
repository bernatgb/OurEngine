#pragma once
#include "Globals.h"

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
	float GetMinX() const {
		return m_MinX;
	};
	float GetMaxX() const {
		return m_MaxX;
	};
	float GetMinY() const {
		return m_MinY;
	};
	float GetMaxY() const {
		return m_MaxY;
	};
	float GetMinZ() const {
		return m_MinZ;
	};
	float GetMaxZ() const {
		return m_MaxZ;
	};

private:
	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ebo;
	unsigned int m_MaterialIndex;
	unsigned int m_NumVertices;
	unsigned int m_NumIndices;

	float m_MinX, m_MaxX;
	float m_MinY, m_MaxY;
	float m_MinZ, m_MaxZ;
};

