#include "SceneImporter.h"

#include <stdlib.h>

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
