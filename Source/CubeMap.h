#pragma once
#include "Globals.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleProgram.h"
#include "ModuleTexture.h"
#include "GL/glew.h"

#include <vector>
#include <string>

class CubeMap
{
public:
	CubeMap() 
	{
		float skyboxVertices[] = {
			-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
		};

		//Creating VBO
		glGenBuffers(1, &m_Vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Creating VAO
		glGenVertexArrays(1, &m_Vao);
		glBindVertexArray(m_Vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);

		//Creating Program
		m_Program = App->program->CreateProgram(".\\assets\\Shaders\\vertex_shader_cubemap.vert", ".\\assets\\Shaders\\fragment_shader_cubemap.frag");
	};
	~CubeMap() 
	{
		for (unsigned int i = 0; i < m_Textures.size(); ++i)
			glDeleteTextures(1, &m_Textures[i]);
		glDeleteBuffers(1, &m_Vbo);
		glDeleteVertexArrays(1, &m_Vao);
		glDeleteProgram(m_Program);
	};

	void LoadTexture(const std::vector<std::string>& _faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < _faces.size(); i++)
		{
			if (!App->texture->LoadTextureData(_faces[i].c_str(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i))
				MY_LOG("Error in CubeMap loading texture: %s", _faces[i]);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		m_Textures.push_back(textureID);
	};
	void Draw(unsigned int index)
	{
		glDepthMask(GL_FALSE);
		glUseProgram(m_Program);
		float4x4 viewMatrixWithoutTrans = float4x4(App->camera->view.Float3x3Part());
		glUniformMatrix4fv(glGetUniformLocation(m_Program, "view"), 1, GL_TRUE, &viewMatrixWithoutTrans[0][0]);
		//glUniformMatrix4fv(glGetUniformLocation(m_Program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(m_Program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_Textures[index]);
		glUniform1i(glGetUniformLocation(m_Program, "skybox"), 0);

		glBindVertexArray(m_Vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
	};


	unsigned int m_Program;
	unsigned int m_Vao;
	unsigned int m_Vbo;
	std::vector<unsigned int> m_Textures;
};

