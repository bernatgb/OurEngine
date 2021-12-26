#pragma once
#include "Globals.h"
#include "Model.h"

namespace importer {
	namespace model {
		void Import(const aiScene* model, Model* ourModel, std::string fullPath);
		int Save(const Model* ourModel, char*& fileBuffer);
		void Load(const char* fileBuffer, Model* ourModel);
	}
}