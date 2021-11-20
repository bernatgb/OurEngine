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

private:
	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ebo;
	unsigned int m_MaterialIndex;
	unsigned int m_NumVertices;
	unsigned int m_NumIndices;
};

