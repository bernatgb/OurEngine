#include "ModuleTexture.h"

#include "IL/il.h"
#include "IL/ilu.h"

ModuleTexture::ModuleTexture()
{
}

ModuleTexture::~ModuleTexture()
{
}

bool ModuleTexture::Init()
{
	ilInit();

	return true;
}

TextureData* ModuleTexture::LoadAndReturnTextureData(const char* source, bool flipIfNeeded)
{
	unsigned int texture;
	ilGenImages(1, &texture);
	ilBindImage(texture);

	if (!ilLoadImage(source))
		return nullptr;

	ILinfo textureInfo;
	iluGetImageInfo(&textureInfo);
	if (textureInfo.Origin == IL_ORIGIN_UPPER_LEFT && flipIfNeeded)
		iluFlipImage();

	byte* data = ilGetData();

	//ilDeleteImages(1, &texture);

	TextureData* textureData = new TextureData(texture, textureInfo.Width, textureInfo.Height, textureInfo.Depth, textureInfo.Format, data);

	return textureData;
}

void ModuleTexture::DeleteTextureData(TextureData* textureData)
{
	ilDeleteImages(1, &textureData->texture);
	
	delete textureData;
}

bool ModuleTexture::LoadTextureData(const char* source, GLenum target)
{
	unsigned int texture;
	ilGenImages(1, &texture);
	ilBindImage(texture);

	if (!ilLoadImage(source))
		return false;

	byte* data = ilGetData();

	ILinfo textureInfo;
	iluGetImageInfo(&textureInfo);
	//if (textureInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	//	iluFlipImage();

	glTexImage2D(target, 0, textureInfo.Format, textureInfo.Width, textureInfo.Height, 0, textureInfo.Format, GL_UNSIGNED_BYTE, data);

	ilDeleteImages(1, &texture);

	return true;
}

bool ModuleTexture::LoadTextureData(const char* source, unsigned int& width, unsigned int& height, unsigned int& depth, unsigned int& format, GLenum target)
{
	unsigned int texture;
	ilGenImages(1, &texture);
	ilBindImage(texture);

	if (!ilLoadImage(source))
		return false;

	byte* data = ilGetData();

	ILinfo textureInfo;
	iluGetImageInfo(&textureInfo);
	if (textureInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		iluFlipImage();
	
	width = textureInfo.Width;
	height = textureInfo.Height;
	depth = textureInfo.Depth;
	format = textureInfo.Format;

	glTexImage2D(target, 0, textureInfo.Format, width, height, 0, textureInfo.Format, GL_UNSIGNED_BYTE, data);

	ilDeleteImages(1, &texture);
	return true;
}
