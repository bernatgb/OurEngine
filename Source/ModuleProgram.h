#pragma once
#include "Module.h"
#include "Globals.h"

// Module -> static (utils)
class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	unsigned int CreateProgram(const char* vtx_shader_source, const char* frg_shader_source) const;
};

