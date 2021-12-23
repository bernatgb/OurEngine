#include "SceneImporter.h"

#include <stdlib.h>
#include <string>
#include <filesystem>
#include <iostream>

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
