#pragma once

#include <string>
#include <Math\float3.h>

struct TextureData;
class Texture
{
public:
	Texture() {};
	Texture(const char* _fileName, const char* _fullPath);
	~Texture();

	void DrawImGui();

	std::string m_Name;
	unsigned int m_GUID;

	unsigned int m_Texture;
	TextureData* m_TextureData;

	unsigned int m_SpecularTexture;
	TextureData* m_SpecularTextureData;

	float3 m_DiffuseColor;
	float3 m_SpecularColor;

	bool m_ShininessAlpha;
	float m_Shininess;

	unsigned int m_MinFilter;
	unsigned int m_MagFilter;
	unsigned int m_Wrap;

private:
	const char* FormatToString(unsigned int _format) const;
	const char* ConfigToString(unsigned int _config) const;
};
