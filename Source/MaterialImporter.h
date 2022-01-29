#pragma once
#include "Globals.h"

#include "Material.h"
#include "Texture.h"

struct aiMaterial;
struct aiString;

namespace importer {
	namespace texture {
		bool Import(const char* filePath, Texture* ourTexture, const char* fullPath);
		int Save(const Texture* ourTexture, char*& fileBuffer);
		void Load(const char* fileBuffer, Texture* ourTexture);
	}

	namespace material {
		void Import(const aiMaterial* material, Material* ourMaterial, const char* fullPath);
		int Save(const Material* ourMaterial, char*& fileBuffer);
		void Load(const char* fileBuffer, Material* ourMaterial);
	}
}
