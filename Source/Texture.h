#pragma once

struct TextureData;
class Texture
{
public:
	Texture(const char* _fileName, const char* _fullPath);
	~Texture();

	void ActivateTexture(const unsigned int& program);

	void DrawImGui();

	char* m_Name = nullptr;

	unsigned int m_Texture;
	TextureData* m_TextureData;

	unsigned int m_MinFilter;
	unsigned int m_MagFilter;
	unsigned int m_Wrap;

private:
	const char* ConfigToString(unsigned int _config) const;
};
