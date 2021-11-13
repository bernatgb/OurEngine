#pragma once
#include "Module.h"

#include "MathGeoLib.h"

class ModuleRenderExercise : public Module
{
public:
	ModuleRenderExercise();
	~ModuleRenderExercise();

	bool Init();
	update_status Update();
	bool CleanUp();

private:
	unsigned int program;
	unsigned int vBuffer;
};

