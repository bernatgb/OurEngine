#pragma once
#include "Module.h"

#include "GL/glew.h"

// Module -> static (utils)
class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture() override;

	bool Init();

	bool LoadTextureData(const char* source, GLenum target = GL_TEXTURE_2D);
	bool LoadTextureData(const char* source, unsigned int& width, unsigned int& height, unsigned int& depth, unsigned int& format, GLenum target = GL_TEXTURE_2D);
};