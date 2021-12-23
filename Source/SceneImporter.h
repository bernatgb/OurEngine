#pragma once
#include "Globals.h"

#include "ModelImporter.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"

#include <map>

namespace importer {
	//void LoadLibraryFiles();
	bool SaveFile(const char* path, const char* data, const int size);
	bool LoadFile(const char* path, char*& data);

	void LoadMeshes(std::map<unsigned int, Mesh*>& _meshes);
	void LoadTextures(std::map<unsigned int, Texture*>& _textures);
	void LoadModels(std::map<std::string, Model*>& _models);
}