#pragma once
#include "Module.h"

#include "GL/glew.h"

struct TextureData {
	TextureData(bool _loadedFromDevil, unsigned int _texture, unsigned int _width, unsigned int _height, unsigned int _depth, unsigned int _format, unsigned int _bpp, byte* _data) {
		loadedFromDevil = _loadedFromDevil;
		texture = _texture;
		width = _width;
		height = _height;
		depth = _depth;
		format = _format;
		bpp = _bpp;
		data = _data;
	}

	bool loadedFromDevil;
	unsigned int texture;
	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int format;
	unsigned int bpp;
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
