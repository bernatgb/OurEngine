#include "SceneImporter.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <stdlib.h>
#include <string>
#include <filesystem>
#include <iostream>

bool importer::SaveFile(const char* path, const rapidjson::Document& jsonDocument)
{
	FILE* file = nullptr;
	fopen_s(&file, path, "wb");

	if (file)
	{
		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> pWriter(buffer);
		jsonDocument.Accept(pWriter);

		const char* jsonFile = buffer.GetString();
		unsigned int size = strlen(jsonFile);

		fwrite(jsonFile, 1, size, file);
		fclose(file);

		return true;
	}

	return false;
}

bool importer::SaveFile(const char* path, const char* data, const int size)
{
	FILE* file = nullptr;
	fopen_s(&file, path, "wb");

	if (file) 
	{
		fwrite(data, 1, size, file);
		fclose(file);

		return true;
	}
	
	return false;
}

bool importer::LoadFile(const char* path, char*& data)
{
	FILE* file = nullptr;
	fopen_s(&file, path, "rb");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		data = (char*)malloc(size + 1);
		fseek(file, 0, SEEK_SET);
		fread(data, 1, size, file);
		data[size] = 0;
		fclose(file);

		return true;
	}

	return false;
}

void importer::SaveResources(const std::map<unsigned int, Mesh*>& _meshes, const std::map<unsigned int, Texture*>& _materials, const std::map<std::string, Model*>& _models)
{
	rapidjson::Document d;
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	d.SetObject();

	// Store Meshes
	rapidjson::Value meshes(rapidjson::kArrayType);
	for (auto it = _meshes.begin(); it != _meshes.end(); ++it) 
	{
		// Path
		std::string path = LIBRARY_MESHES + std::to_string(it->first) + ".asset";

		// Store mesh
		char* file = nullptr;
		int fileSize = importer::mesh::Save(it->second, file);
		importer::SaveFile(path.c_str(), file, fileSize);
		delete[] file;

		// Store mesh resource info
		rapidjson::Value newMesh(rapidjson::kObjectType);
		newMesh.AddMember("Id", it->first, allocator);
		newMesh.AddMember("Path", rapidjson::Value(path.c_str(), allocator), allocator);
		meshes.PushBack(newMesh, allocator);
	}
	d.AddMember("Meshes", meshes, allocator);

	// Store Materials
	rapidjson::Value materials(rapidjson::kArrayType);
	for (auto it = _materials.begin(); it != _materials.end(); ++it)
	{
		// Path
		std::string path = LIBRARY_MATERIALS + std::to_string(it->first) + ".asset";

		// Store material
		char* file = nullptr;
		int fileSize = importer::material::Save(it->second, file);
		importer::SaveFile(path.c_str(), file, fileSize);
		delete[] file;

		// Store material resource info
		rapidjson::Value newMaterial(rapidjson::kObjectType);
		newMaterial.AddMember("Id", it->first, allocator);
		newMaterial.AddMember("Path", rapidjson::Value(path.c_str(), allocator), allocator);
		materials.PushBack(newMaterial, allocator);
	}
	d.AddMember("Materials", materials, allocator);

	// Store Models
	rapidjson::Value models(rapidjson::kArrayType);
	for (auto it = _models.begin(); it != _models.end(); ++it)
	{
		// Path
		std::string path = LIBRARY_MODELS + it->first + ".asset";

		// Store model
		char* file = nullptr;
		int fileSize = importer::model::Save(it->second, file);
		importer::SaveFile(path.c_str(), file, fileSize);
		delete[] file;

		// Store model resource info
		rapidjson::Value newModel(rapidjson::kObjectType);
		newModel.AddMember("Id", rapidjson::Value(it->first.c_str(), allocator), allocator);
		newModel.AddMember("Path", rapidjson::Value(path.c_str(), allocator), allocator);
		models.PushBack(newModel, allocator);
	}
	d.AddMember("Models", models, allocator);

	// Store resources file
	importer::SaveFile(RESOURCES_FILE, d);
}

void importer::LoadResources(std::map<unsigned int, Mesh*>& _meshes, std::map<unsigned int, Texture*>& _materials, std::map<std::string, Model*>& _models)
{
	// Load resources file
	char* fileBuffer = nullptr;
	importer::LoadFile(RESOURCES_FILE, fileBuffer);

	rapidjson::Document d;
	d.Parse(fileBuffer);

	delete[] fileBuffer;

	// Load Meshes
	for (rapidjson::Value::ConstValueIterator itr = d["Meshes"].Begin(); itr != d["Meshes"].End(); ++itr)
	{
		// Get mesh resource info
		unsigned int id = (*itr)["Id"].GetInt();
		std::string path = (*itr)["Path"].GetString();

		// Load and store mesh
		char* file = nullptr;
		importer::LoadFile(path.c_str(), file);

		Mesh* mesh = new Mesh();
		importer::mesh::Load(file, mesh);
		_meshes[id] = mesh;

		delete[] file;
	}

	// Load Materials
	for (rapidjson::Value::ConstValueIterator itr = d["Materials"].Begin(); itr != d["Materials"].End(); ++itr)
	{
		// Get material resource info
		unsigned int id = (*itr)["Id"].GetInt();
		std::string path = (*itr)["Path"].GetString();

		// Load and store material
		char* file = nullptr;
		importer::LoadFile(path.c_str(), file);

		Texture* material = new Texture();
		importer::material::Load(file, material);
		_materials[id] = material;

		delete[] file;
	}

	// Load Models
	for (rapidjson::Value::ConstValueIterator itr = d["Models"].Begin(); itr != d["Models"].End(); ++itr)
	{
		// Get model resource info
		std::string id = (*itr)["Id"].GetString();
		std::string path = (*itr)["Path"].GetString();

		// Load and store model
		char* file = nullptr;
		importer::LoadFile(path.c_str(), file);

		Model* model = new Model();
		importer::model::Load(file, model);
		_models[id] = model;

		delete[] file;
	}
}


void importer::LoadMeshes(std::map<unsigned int, Mesh*>& _meshes)
{
	std::string path = ".\\Assets\\Library\\Meshes";

	//for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		/*char* data = nullptr;
		LoadFile(entry.path(), data);
		Mesh* mesh = new Mesh();
		mesh::Load(data, mesh);
		delete[] data;

		_meshes[mesh->m_GUID] = mesh;*/
	}
}

void importer::LoadTextures(std::map<unsigned int, Texture*>& _textures)
{
}

void importer::LoadModels(std::map<std::string, Model*>& _models)
{
}
