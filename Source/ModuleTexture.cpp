#include "ModuleTexture.h"

#include "IL/il.h"

ModuleTexture::ModuleTexture()
{
}

ModuleTexture::~ModuleTexture()
{
}

bool ModuleTexture::Init()
{
	ilInit();

	ilGenImages(1, &lenna);
	ilBindImage(lenna);
	ilLoadImage("..\\Source\\resources\\Lenna.png");
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	data = ilGetData();

	/*ILvoid iluGetImageInfo(
		ILinfo * Info
	);*/

	return true;
}

bool ModuleTexture::CleanUp()
{
	ilDeleteImages(1, &lenna);

	return true;
}
