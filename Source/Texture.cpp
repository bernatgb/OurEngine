#include "Texture.h"
#include "Application.h"
#include "ModuleTexture.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"
#include <string>

//TODO: STRING TO CHAR*

Texture::Texture(const char* _fileName, const char* _fullPath)
{
	MY_LOG("Assimp texture (%s): Loading the texture file and setting its configuration", _fileName);
	m_Name = _fileName;

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	MY_LOG("Assimp texture: Loading model from the path described in the FBX");
	if (!App->texture->LoadTextureData(_fileName)) 
	{
		MY_LOG("Assimp texture: Loading model from the same folder you of the FBX");

		std::string directoryPath = _fullPath;
		directoryPath = directoryPath.substr(0, directoryPath.rfind('\\'));
		directoryPath += '\\' + _fileName;

		if (!App->texture->LoadTextureData(_fileName))
		{

			directoryPath = TEXTURES_FOLDER;
			directoryPath += _fileName;

			MY_LOG("Assimp texture: Loading model from the Textures/ folder");
			if (!App->texture->LoadTextureData(_fileName))
			{
				MY_LOG("Assimp texture: Texture couldn't be loaded");
				return;
			}
		}
	}

	MY_LOG("Assimp texture: Texture loaded correctly");
	
	//TODO: To find a texture try (LOG every step):
	//a.First check on the path described in the FBX
	//b.Then check on the same folder you loaded the FBX
	//c.Last, try in your own “Textures / ” folder

	//glGenerateTextureMipmap(texture);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_Texture);

	delete[] m_Name;
}

void Texture::ActivateTexture(const unsigned int& program) 
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
}