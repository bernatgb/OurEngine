#pragma once
#include "Globals.h"

#include "Mesh.h"
#include "Texture.h"

#include <vector>

class Model
{
public:
	Model(const char* _fileName);
	~Model();

	void Draw(const unsigned int& _program) const;

	void DrawImGui();

private:
	void LoadMeshes(aiMesh** _meshes, const unsigned int& _numMeshes);
	void LoadTextures(aiMaterial** _materials, const unsigned int& _numMaterials);

	const char* m_Name;
	std::vector<Mesh*> m_Meshes;
	std::vector<Texture*> m_Textures;

	unsigned int m_NumVertices = 0;
	unsigned int m_NumTriangles = 0;

};

