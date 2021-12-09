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
