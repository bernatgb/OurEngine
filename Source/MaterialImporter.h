#pragma once
#include "Globals.h"

#include "Texture.h"

struct aiMaterial;

namespace importer {
	namespace material {
		void Import(const aiMaterial* material, Texture* ourMaterial, const char* fullPath);
		int Save(const Texture* ourMaterial, char*& fileBuffer);
		void Load(const char* fileBuffer, Texture* ourMaterial);
	}
}