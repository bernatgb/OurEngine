#pragma once
#include "Module.h"

// Module -> static (utils)
class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram() override;

	unsigned int CreateProgram(const char* vtx_shader_source, const char* frg_shader_source) const;
};

