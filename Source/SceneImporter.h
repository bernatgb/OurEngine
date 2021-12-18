#pragma once
#include "Globals.h"

#include "MeshImporter.h"
#include "MaterialImporter.h"

namespace importer {
	//void LoadLibraryFiles();
	bool SaveFile(const char* path, const char* data, const int size);
	bool LoadFile(const char* path, char*& data);
}