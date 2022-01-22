#pragma once

#include <string>
#include <Math\float3.h>

struct TextureData;
class Texture
{
public:
	Texture();
	~Texture();

	void DrawImGui();

	std::string m_Name = "";
	unsigned int m_GUID = 0;

	unsigned int m_Texture = 0;
	TextureData* m_TextureData = nullptr;

	unsigned int m_SpecularTexture = 0;
	TextureData* m_SpecularTextureData = nullptr;

	float3 m_DiffuseColor = float3(0.3f);
	float3 m_SpecularColor = float3(1.0f);

	bool m_ShininessAlpha = false;
	float m_Shininess = 0.0f;

	unsigned int m_MinFilter = 0x2601;
	unsigned int m_MagFilter = 0x2601;
	unsigned int m_Wrap = 0x2900;

private:
	const char* FormatToString(unsigned int _format) const;
	const char* ConfigToString(unsigned int _config) const;
};
