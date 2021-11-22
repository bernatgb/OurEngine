#include "Texture.h"
#include "Application.h"
#include "ModuleTexture.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"
#include <string>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

//TODO: STRING TO CHAR*

Texture::Texture(const char* _fileName, const char* _fullPath)
{
	MY_LOG("Assimp texture (%s): Loading the texture file and setting its configuration", _fileName);
	
	m_Name = new char[strlen(_fileName)];
	strcpy(m_Name, _fileName);

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	MY_LOG("Assimp texture: Loading model from the path described in the FBX");
	if (!App->texture->LoadTextureData(_fileName, width, height, depth, format))
	{
		MY_LOG("Assimp texture: Loading model from the same folder you of the FBX");

		std::string directoryPath = _fullPath;
		directoryPath = directoryPath.substr(0, directoryPath.rfind('\\'));
		directoryPath += '\\' + _fileName;

		if (!App->texture->LoadTextureData(directoryPath.c_str(), width, height, depth, format))
		{

			directoryPath = TEXTURES_FOLDER;
			directoryPath += _fileName;

			MY_LOG("Assimp texture: Loading model from the Textures/ folder");
			if (!App->texture->LoadTextureData(directoryPath.c_str(), width, height, depth, format))
			{
				MY_LOG("Assimp texture: Texture couldn't be loaded");
				return;
			}
		}
	}

	MY_LOG("Assimp texture: Texture loaded correctly");
	
	//glGenerateTextureMipmap(texture);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_Texture);

	//delete[] m_Name;
}

void Texture::ActivateTexture(const unsigned int& program) 
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
}

void Texture::DrawImGui()
{
	if (ImGui::CollapsingHeader(m_Name))
	{
		ImGui::Text("Witdh: %i", width);
		ImGui::Text("Height: %i", height);
		ImGui::Text("Depth: %i", depth);

		switch (format)
		{
		case IL_COLOUR_INDEX:
			ImGui::Text("Format: COLOUR_INDEX");
			break;
		case IL_RGB:
			ImGui::Text("Format: RGB");
			break;
		case IL_RGBA:
			ImGui::Text("Format: RGBA");
			break;
		case IL_BGR:
			ImGui::Text("Format: BGR");
			break;
		case IL_BGRA:
			ImGui::Text("Format: BGRA");
			break;
		case IL_LUMINANCE:
			ImGui::Text("Format: LUMINANCE");
			break;
		}
	}
}
