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

	void Draw();

private:
	std::vector<Mesh*> m_Meshes;
	std::vector<Texture*> m_Textures;

	unsigned int m_NumMeshes;
};

