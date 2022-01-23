#include "CubeMap.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleProgram.h"
#include "ModuleTexture.h"
#include "GL/glew.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

CubeMap::CubeMap()
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
}

CubeMap::~CubeMap()
{
	for (unsigned int i = 0; i < m_Textures.size(); ++i)
		glDeleteTextures(1, &m_Textures[i]);
	glDeleteBuffers(1, &m_Vbo);
	glDeleteVertexArrays(1, &m_Vao);
	glDeleteProgram(m_Program);
}

void CubeMap::LoadTexture(const std::vector<std::string>& _faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < _faces.size(); i++)
	{
		TextureData* textureData = App->texture->LoadAndReturnTextureData(_faces[i].c_str(), false);
		if (textureData != nullptr)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, textureData->format, textureData->width, textureData->height, 0, textureData->format, GL_UNSIGNED_BYTE, textureData->data);
			App->texture->DeleteTextureData(textureData);

			m_TexturesNames.push_back(_faces[i]);
		}
		else
		{
			MY_LOG("Error in CubeMap loading texture: %s", _faces[i]);
		}			
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	m_Textures.push_back(textureID);
}

void CubeMap::Draw(unsigned int index)
{
	glDepthMask(GL_FALSE);
	glUseProgram(m_Program);
	float4x4 viewMatrixWithoutTrans = float4x4(App->camera->GetView().Float3x3Part());
	glUniformMatrix4fv(glGetUniformLocation(m_Program, "view"), 1, GL_TRUE, &viewMatrixWithoutTrans[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation(m_Program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_Program, "proj"), 1, GL_TRUE, &App->camera->GetProj()[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_Textures[index]);
	glUniform1i(glGetUniformLocation(m_Program, "skybox"), 0);

	glBindVertexArray(m_Vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
}

void CubeMap::DrawImGui()
{
	if (ImGui::CollapsingHeader("Skybox"))
	{

		ImGui::Image((void*)m_Textures[0], ImVec2(100, 100));
		ImGui::Text("Textures used for the skybox:");
		for (int i = 0; i < m_TexturesNames.size(); ++i)
		{
			ImGui::BulletText("%s:", m_TexturesNames[i].c_str());

			TextureData* textureInfo = App->texture->LoadAndReturnTextureData(m_TexturesNames[i].c_str(), false);

			ImGui::Image((void*)textureInfo->texture, ImVec2(50, 50));
			App->texture->DeleteTextureData(textureInfo);
			// TODO: change skybox cubemap files using imgui
		}
	}
}