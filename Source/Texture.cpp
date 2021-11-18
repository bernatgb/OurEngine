#include "Texture.h"

Texture::Texture()
{
	/*
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	MY_LOG("Textures: Reading the texture file and setting its data");
	unsigned int devILTexture;
	int width, height;
	byte* data = nullptr;
	App->texture->GetTextureData(devILTexture, "..\\Source\\resources\\Lenna.png", width, height, data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	App->texture->CleanTexture(devILTexture);
	glGenerateTextureMipmap(texture);
	*/
}

Texture::~Texture()
{
}
