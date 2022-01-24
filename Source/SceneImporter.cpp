#include "SceneImporter.h"

#include "Model.h"
#include "ModelImporter.h"

#include <assimp/Importer.hpp>
#include "assimp/ai_assert.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <stdlib.h>
#include <string>
#include <filesystem>
#include <iostream>

Model* importer::LoadModel(const char* path)
{
	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate);
	//const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate);
	if (scene)
	{
		Model* model = new Model();
		importer::model::Import(scene, model, path);
		return model;
	}

	return nullptr;
}

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

bool importer::LoadFile(const char* path, rapidjson::Document& jsonDocument)
{
	FILE* file = nullptr;
	fopen_s(&file, path, "rb");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		char* data = (char*)malloc(size + 1);
		fseek(file, 0, SEEK_SET);
		fread(data, 1, size, file);
		data[size] = 0;
		fclose(file);

		jsonDocument.Parse(data);
		free(data);

		return true;
	}

	return false;
}

void importer::SaveResources(const std::map<unsigned int, Mesh*>& _meshes, const std::map<unsigned int, Texture*>& _texture, const std::map<unsigned int, Material*>& _materials, const std::map<std::string, Model*>& _models)
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

	// Store Textures
	rapidjson::Value textures(rapidjson::kArrayType);
	for (auto it = _texture.begin(); it != _texture.end(); ++it)
	{
		// Path
		std::string path = LIBRARY_TEXTURES + std::to_string(it->first) + ".asset";

		// Store texture
		char* file = nullptr;
		int fileSize = importer::texture::Save(it->second, file);
		importer::SaveFile(path.c_str(), file, fileSize);
		delete[] file;

		// Store material resource info
		rapidjson::Value newTexture(rapidjson::kObjectType);
		newTexture.AddMember("Id", it->first, allocator);
		newTexture.AddMember("Path", rapidjson::Value(path.c_str(), allocator), allocator);
		textures.PushBack(newTexture, allocator);
	}
	d.AddMember("Textures", textures, allocator);

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

void importer::LoadResources(std::map<unsigned int, Mesh*>& _meshes, std::map<unsigned int, Texture*>& _textures, std::map<unsigned int, Material*>& _materials, std::map<std::string, Model*>& _models)
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

	// Load Textures
	for (rapidjson::Value::ConstValueIterator itr = d["Textures"].Begin(); itr != d["Textures"].End(); ++itr)
	{
		// Get material resource info
		unsigned int id = (*itr)["Id"].GetInt();
		std::string path = (*itr)["Path"].GetString();

		// Load and store material
		char* file = nullptr;
		importer::LoadFile(path.c_str(), file);

		Texture* texture = new Texture();
		importer::texture::Load(file, texture);
		_textures[id] = texture;

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

		Material* material = new Material();
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

// UTILS

float3 importer::ValueToFloat3(const rapidjson::Value& value)
{
	rapidjson::Value::ConstValueIterator itr = value.Begin();
	float x = (itr++)->GetFloat();
	float y = (itr++)->GetFloat();
	float z = (itr++)->GetFloat();
	return float3(x, y, z);
	
	//return float3((itr++)->GetFloat(), (itr++)->GetFloat(), (itr++)->GetFloat());
}

rapidjson::Value importer::Float3ToValue(const float3& value, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value vector(rapidjson::kArrayType);

	/*minPoint.Reserve(3, allocator);
	minPoint[0] = ourModel->m_Min.x;
	minPoint[1] = ourModel->m_Min.y;
	minPoint[2] = ourModel->m_Min.z;*/

	vector.PushBack(value.x, allocator);
	vector.PushBack(value.y, allocator);
	vector.PushBack(value.z, allocator);
	return vector;
}

Quat importer::ValueToQuat(const rapidjson::Value& value)
{
	rapidjson::Value::ConstValueIterator itr = value.Begin();
	float x = (itr++)->GetFloat();
	float y = (itr++)->GetFloat();
	float z = (itr++)->GetFloat();
	float w = (itr++)->GetFloat();
	return Quat(x, y, z, w);

	//return Quat((itr++)->GetFloat(), (itr++)->GetFloat(), (itr++)->GetFloat(), (itr++)->GetFloat());
}

rapidjson::Value importer::QuatToValue(const Quat& value, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value quat(rapidjson::kArrayType);
	quat.PushBack(value.x, allocator);
	quat.PushBack(value.y, allocator);
	quat.PushBack(value.z, allocator);
	quat.PushBack(value.w, allocator);
	return quat;
}
