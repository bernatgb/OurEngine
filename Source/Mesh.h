#pragma once
#include "Globals.h"

#include "assimp/cimport.h"
#include "Assimp/scene.h"

class Mesh
{
public:
	Mesh(aiMesh* _mesh);
	~Mesh();

	void Draw();

private:
	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ebo;
	//material_index
	unsigned int m_NumVertices;
	unsigned int m_NumIndices;
};

