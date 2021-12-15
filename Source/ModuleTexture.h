#pragma once
#include "Module.h"

#include "GL/glew.h"

struct TextureData {
	TextureData(unsigned int _texture, unsigned int _width, unsigned int _height, unsigned int _depth, unsigned int _format, byte* _data) {
		texture = _texture;
		width = _width;
		height = _height;
		depth = _depth;
		format = _format;
		data = _data;
	}

	unsigned int texture;
	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int format;
	byte* data;
};

// Module -> static (utils)
class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture() override;

	bool Init();

	TextureData* LoadAndReturnTextureData(const char* source, bool flipIfNeeded = true);
	void DeleteTextureData(TextureData* textureData);
	bool LoadTextureData(const char* source, GLenum target = GL_TEXTURE_2D);
	bool LoadTextureData(const char* source, unsigned int& width, unsigned int& height, unsigned int& depth, unsigned int& format, GLenum target = GL_TEXTURE_2D);
};