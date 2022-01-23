#include "Texture.h"
#include "Application.h"
#include "ModuleTexture.h"

#include "GL/glew.h"
#include <string>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "MaterialImporter.h"

Texture::Texture()
{
}

Texture::~Texture()
{
	App->texture->DeleteTextureData(m_TextureData);
	glDeleteTextures(1, &m_Texture);
}

void Texture::DrawImGui()
{
	if (m_TextureData == nullptr)
		return;

	ImGui::Text("DiffuseTexture");
	ImGui::Text("Name: %s", m_Name.c_str());
	ImGui::Text("Witdh: %i", m_TextureData->width);
	ImGui::Text("Height: %i", m_TextureData->height);
	ImGui::Text("Depth: %i", m_TextureData->depth);
	ImGui::Text("Format: %s", FormatToString(m_TextureData->format));

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

const char* Texture::FormatToString(unsigned int _format) const
{
	switch (_format)
	{
	case GL_COLOR_INDEX: return "COLOUR_INDEX";
	case GL_RGB: return "Format: RGB";
	case GL_RGBA: return "Format: RGBA";
	case GL_BGR: return "Format: BGR";
	case GL_BGRA: return "Format: BGRA";
	case GL_LUMINANCE: return "Format: LUMINANCE";
	}
	return "";
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
	return "";
}