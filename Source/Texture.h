#pragma once
class Texture
{
public:
	Texture(const char* _fileName, const char* _fullPath);
	~Texture();

	void ActivateTexture(const unsigned int& program);

	void DrawImGui();

private:
	const char* ConfigToString(unsigned int _config) const;

	unsigned int m_Texture;

	char* m_Name = nullptr;
	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int format;

	unsigned int m_MinFilter;
	unsigned int m_MagFilter;
	unsigned int m_Wrap;
};
