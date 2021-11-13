#pragma once
#include "Module.h"

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	bool Init();
	bool CleanUp();

	unsigned int lenna;
	int width;
	int height;
	byte* data;
};