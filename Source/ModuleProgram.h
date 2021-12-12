#pragma once
#include "Module.h"

#include <vector>

enum class Shaders {
	TEXTURE,
	PHONG,
	CUBE_MAP
};

// Module -> static (utils)
class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram() override;

	bool Init();

	unsigned int CreateProgram(const char* vtx_shader_source, const char* frg_shader_source) const;

	std::vector<unsigned int> m_Shaders;
};

