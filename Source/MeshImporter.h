#pragma once
#include "Globals.h"
#include "Mesh.h"

namespace importer {
namespace mesh {
	void Import(const aiMesh* mesh, Mesh* ourMesh);
	int Save(const Mesh* ourMesh, char*& fileBuffer);
	void Load(const char* fileBuffer, Mesh* ourMesh);
}
}
