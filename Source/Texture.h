#pragma once

#include <string>

struct TextureData;
class Texture
{
public:
	Texture() {};
	Texture(const char* _fileName, const char* _fullPath);
	~Texture();

	void ActivateTexture(const unsigned int& program);

	void DrawImGui();

	std::string m_Name;
	unsigned int m_GUID;

	unsigned int m_Texture;
	TextureData* m_TextureData;

	unsigned int m_MinFilter;
	unsigned int m_MagFilter;
	unsigned int m_Wrap;

private:
	const char* ConfigToString(unsigned int _config) const;
};
