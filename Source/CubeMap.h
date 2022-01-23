#pragma once

#include <vector>
#include <string>

class CubeMap
{
public:
	CubeMap();
	~CubeMap();

	void LoadTexture(const std::vector<std::string>& _faces);
	void Draw(unsigned int index);
	void DrawImGui();

private:
	unsigned int m_Program;
	unsigned int m_Vao;
	unsigned int m_Vbo;
	std::vector<unsigned int> m_Textures;
	std::vector<std::string> m_TexturesNames;

};

