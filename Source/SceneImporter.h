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
	Model* LoadModel(const char* path);

	bool SaveFile(const char* path, const rapidjson::Document& jsonDocument);
	bool SaveFile(const char* path, const char* data, const int size);
	bool LoadFile(const char* path, char*& data);
	bool LoadFile(const char* path, rapidjson::Document& jsonDocument);

	void SaveResources(const std::map<unsigned int, Mesh*>& _meshes, const std::map<unsigned int, Texture*>& _materials, const std::map<std::string, Model*>& _models);
	void LoadResources(std::map<unsigned int, Mesh*>& _meshes, std::map<unsigned int, Texture*>& _materials, std::map<std::string, Model*>& _models);

	void LoadMeshes(std::map<unsigned int, Mesh*>& _meshes);
	void LoadTextures(std::map<unsigned int, Texture*>& _textures);
	void LoadModels(std::map<std::string, Model*>& _models);

	// UTILS
	float3 ValueToFloat3(const rapidjson::Value& value);
	rapidjson::Value Float3ToValue(const float3& value, rapidjson::Document::AllocatorType& allocator);
	Quat ValueToQuat(const rapidjson::Value& value);
	rapidjson::Value QuatToValue(const Quat& value, rapidjson::Document::AllocatorType& allocator);
}