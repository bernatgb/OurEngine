#include "Texture.h"
#include "Application.h"
#include "ModuleTexture.h"

#include "GL/glew.h"
#include <string>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

//TODO: STRING TO CHAR*

Texture::Texture(const char* _fileName, const char* _fullPath)
{
	MY_LOG("Assimp texture (%s): Loading the texture file and setting its configuration", _fileName);
	
	m_Name = new char[strlen(_fileName)+1];
	strcpy(m_Name, _fileName);

	m_MinFilter = GL_LINEAR;
	m_MagFilter = GL_LINEAR;
	m_Wrap = GL_CLAMP;

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Wrap);

	MY_LOG("Assimp texture (%s): Loading model from the path described in the FBX", _fileName);
	if (!App->texture->LoadTextureData(_fileName, width, height, depth, format))
	{
		std::string directoryPath = _fullPath;
		const size_t last_slash_idx = directoryPath.rfind('\\');
		if (std::string::npos != last_slash_idx)
			directoryPath = directoryPath.substr(0, last_slash_idx) + '\\' + _fileName;
		
		MY_LOG("Assimp texture (%s): Loading model from the same folder you of the FBX", directoryPath.c_str());
		if (!App->texture->LoadTextureData(directoryPath.c_str(), width, height, depth, format))
		{
			directoryPath = TEXTURES_FOLDER;
			directoryPath += _fileName;

			MY_LOG("Assimp texture (%s): Loading model from the Textures/ folder", directoryPath.c_str());
			if (!App->texture->LoadTextureData(directoryPath.c_str(), width, height, depth, format))
			{
				MY_LOG("Assimp texture: Texture couldn't be loaded");
				return;
			}
		}
	}

	MY_LOG("Assimp texture: Texture loaded correctly");
	
	glGenerateTextureMipmap(m_Texture);
}

Texture::~Texture()
{
	delete[] m_Name;

	glDeleteTextures(1, &m_Texture);
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
		case GL_COLOR_INDEX:
			ImGui::Text("Format: COLOUR_INDEX");
			break;
		case GL_RGB:
			ImGui::Text("Format: RGB");
			break;
		case GL_RGBA:
			ImGui::Text("Format: RGBA");
			break;
		case GL_BGR:
			ImGui::Text("Format: BGR");
			break;
		case GL_BGRA:
			ImGui::Text("Format: BGRA");
			break;
		case GL_LUMINANCE:
			ImGui::Text("Format: LUMINANCE");
			break;
		}

		ImGui::Image((void*)m_Texture, ImVec2(100, 100));

		ImGui::Separator();

		bool change = false;

		if (ImGui::Button("Select Min filter.."))
			ImGui::OpenPopup("min_filter_popup");
		ImGui::SameLine();
		ImGui::TextUnformatted(ConfigToString(m_MinFilter));
		if (ImGui::BeginPopup("min_filter_popup"))
		{
			ImGui::Text("Min Filter");
			ImGui::Separator();
			if (ImGui::Selectable("GL_NEAREST"))
			{
				m_MinFilter = GL_NEAREST;
				change = true;
			}
			if (ImGui::Selectable("GL_LINEAR"))
			{
				m_MinFilter = GL_LINEAR;
				change = true;
			}
			if (ImGui::Selectable("GL_NEAREST_MIPMAP_NEAREST"))
			{
				m_MinFilter = GL_NEAREST_MIPMAP_NEAREST;
				change = true;
			}
			if (ImGui::Selectable("GL_LINEAR_MIPMAP_NEAREST"))
			{
				m_MinFilter = GL_LINEAR_MIPMAP_NEAREST;
				change = true;
			}
			if (ImGui::Selectable("GL_NEAREST_MIPMAP_LINEAR"))
			{
				m_MinFilter = GL_NEAREST_MIPMAP_LINEAR;
				change = true;
			}
			if (ImGui::Selectable("GL_LINEAR_MIPMAP_LINEAR"))
			{
				m_MinFilter = GL_LINEAR_MIPMAP_LINEAR;
				change = true;
			}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Select Mag filter.."))
			ImGui::OpenPopup("mag_filter_popup");
		ImGui::SameLine();
		ImGui::TextUnformatted(ConfigToString(m_MagFilter));
		if (ImGui::BeginPopup("mag_filter_popup"))
		{
			ImGui::Text("Mag Filter");
			ImGui::Separator();
			if (ImGui::Selectable("GL_NEAREST"))
			{
				m_MagFilter = GL_NEAREST;
				change = true;
			}
			if (ImGui::Selectable("GL_LINEAR"))
			{
				m_MagFilter = GL_LINEAR;
				change = true;
			}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Select Wrap.."))
			ImGui::OpenPopup("wrap_popup");
		ImGui::SameLine();
		ImGui::TextUnformatted(ConfigToString(m_Wrap));
		if (ImGui::BeginPopup("wrap_popup"))
		{
			ImGui::Text("Wrap");
			ImGui::Separator();
			if (ImGui::Selectable("GL_CLAMP"))
			{
				m_Wrap = GL_CLAMP;
				change = true;
			}
			if (ImGui::Selectable("GL_REPEAT"))
			{
				m_Wrap = GL_REPEAT;
				change = true;
			}
			ImGui::EndPopup();
		}

		if (change) 
		{
			glBindTexture(GL_TEXTURE_2D, m_Texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Wrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Wrap);
		}
	}
}

const char* Texture::ConfigToString(unsigned int _config) const
{
	switch (_config) {
	case GL_NEAREST: return "GL_NEAREST";
	case GL_LINEAR: return "GL_LINEAR";
	case GL_NEAREST_MIPMAP_NEAREST: return "GL_NEAREST_MIPMAP_NEAREST";
	case GL_LINEAR_MIPMAP_NEAREST: return "GL_LINEAR_MIPMAP_NEAREST";
	case GL_NEAREST_MIPMAP_LINEAR: return "GL_NEAREST_MIPMAP_LINEAR";
	case GL_LINEAR_MIPMAP_LINEAR: return "GL_LINEAR_MIPMAP_LINEAR";
	case GL_CLAMP: return "GL_CLAMP";
	case GL_REPEAT: return "GL_REPEAT";
	}
}