#pragma once
#include "Module.h"

#include <vector>

enum class Shaders {
	TEXTURE,
	PHONG,
	CUBE_MAP
};

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram() override;

	bool Init();

	unsigned int CreateProgram(const char* vtx_shader_source, const char* frg_shader_source) const;
	unsigned int GetProgram(const Shaders _shader) const
	{
		return m_Shaders[(int)_shader];
	};

	std::vector<unsigned int> m_Shaders;
};

