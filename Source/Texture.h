#pragma once
class Texture
{
public:
	Texture(const char* _fileName, const char* _fullPath);
	~Texture();

	void ActivateTexture(const unsigned int& program);

	const char* GetName() {
		return m_Name;
	};

private:
	const char* m_Name;

	unsigned int m_Texture;
};

