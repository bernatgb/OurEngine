#pragma once
#include "Module.h"

// Module -> static (utils)
class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	bool Init();

	bool LoadTextureData(const char* source);

	void GetLastTextureInfo(unsigned int& width, unsigned int& height, unsigned int& depth, unsigned int& format) const;

private:
	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int format;
};