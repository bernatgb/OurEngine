#pragma once
#include "Globals.h"

#include "ModelImporter.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"

#include <map>

#include "rapidjson/document.h"
//namespace rapidjson { typedef GenericDocument<UTF8<> > Document; }

namespace importer {
	//void LoadLibraryFiles();
	bool SaveFile(const char* path, const rapidjson::Document& jsonDocument);
	bool SaveFile(const char* path, const char* data, const int size);
	bool LoadFile(const char* path, char*& data);

	void SaveResources(const std::map<unsigned int, Mesh*>& _meshes, const std::map<unsigned int, Texture*>& _materials, const std::map<std::string, Model*>& _models);
	void LoadResources(std::map<unsigned int, Mesh*>& _meshes, std::map<unsigned int, Texture*>& _materials, std::map<std::string, Model*>& _models);

	void LoadMeshes(std::map<unsigned int, Mesh*>& _meshes);
	void LoadTextures(std::map<unsigned int, Texture*>& _textures);
	void LoadModels(std::map<std::string, Model*>& _models);
}