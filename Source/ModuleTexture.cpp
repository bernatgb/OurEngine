#include "ModuleTexture.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

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

bool ModuleTexture::LoadTextureData(const char* source)
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

	//IL_COLOUR_INDEX, IL_RGB, IL_RGBA, IL_BGR, IL_BGRA, IL_LUMINANCE

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureInfo.Width, textureInfo.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	ilDeleteImages(1, &texture);
	return true;
}

bool ModuleTexture::LoadTextureData(const char* source, unsigned int& width, unsigned int& height, unsigned int& depth, unsigned int& format)
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
	//IL_COLOUR_INDEX, IL_RGB, IL_RGBA, IL_BGR, IL_BGRA, IL_LUMINANCE

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	ilDeleteImages(1, &texture);
	return true;
}
