#include "Application.h"
#include "ModuleRenderExercise.h"
#include "ModuleCamera.h"
#include "ModuleProgram.h"
#include "ModuleTexture.h"
#include "GL/glew.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

ModuleRenderExercise::ModuleRenderExercise()
{
	/*minFilter = new unsigned int[6]{
		GL_NEAREST, GL_LINEAR,
		GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
	};
	magFilter = new unsigned int[2]{
		GL_NEAREST, GL_LINEAR
	};
	wrap = new unsigned int[2]{
		GL_CLAMP, GL_REPEAT
	};

	minPar = 1;
	magPar = 1;
	wrapPar = 0;*/
}

ModuleRenderExercise::~ModuleRenderExercise()
{	
	/*delete[] minFilter;
	delete[] magFilter;
	delete[] wrap;*/
}

bool ModuleRenderExercise::Init()
{
	MY_LOG("Buffer: Creating vertex buffer");
	GLfloat vertex[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, // texcoords
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)(sizeof(float) * 3 * 4) // buffer offset
	);

	MY_LOG("Textures: Generating texture and setting its parameters");
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	MY_LOG("Textures: Reading the texture file and setting its data");
	unsigned int devILTexture;
	int width, height;
	byte* data = nullptr;
	App->texture->GetTextureData(devILTexture, "..\\Source\\resources\\Lenna.png", width, height, data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	App->texture->CleanTexture(devILTexture);
	glGenerateTextureMipmap(texture);

	MY_LOG("Shaders: Creating program");
	program = App->program->CreateProgram("..\\Source\\shaders\\vertex_shader.vert", "..\\Source\\shaders\\fragment_shader.frag");

	model = float4x4::FromTRS(float3(2.0f, 0.0f, 0.0f),
		float4x4::RotateZ(pi / 4.0f),
		float3(2.0f, 1.0f, 0.0f));

	return true;
}

update_status ModuleRenderExercise::Update()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableVertexAttribArray(0);

	return UPDATE_CONTINUE;
}

bool ModuleRenderExercise::CleanUp()
{
	glDeleteBuffers(1, &vBuffer);

	glDeleteProgram(program);

	glDeleteTextures(1, &texture);

	return true;
}

void ModuleRenderExercise::DrawTextureImGui(bool& showWindow)
{
	ImGui::Begin("Texture info", &showWindow);
	unsigned int width, height, depth, format;
	App->texture->GetLastTextureInfo(width, height, depth, format);
	ImGui::Text("Witdh: %i", width);
	ImGui::Text("Height: %i", height);
	ImGui::Text("Depth: %i", depth);
	ImGui::Text("Format: %i", format);

	if (ImGui::BeginTabBar("Options"))
	{
		/*if (ImGui::Button("Select Min filter.."))
			ImGui::OpenPopup("min_filter_popup");
		ImGui::SameLine();
		ImGui::TextUnformatted(minPar == -1 ? "<None>" : minFilterName[minPar]);
		if (ImGui::BeginPopup("min_filter_popup"))
		{
			ImGui::Text("Min Filter");
			ImGui::Separator();
			for (int i = 0; i < 6; i++)
				if (ImGui::Selectable(minFilterName[i]))
				{
					minPar = i;
					SetTextureParameters();
				}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Select Mag filter.."))
			ImGui::OpenPopup("mag_filter_popup");
		ImGui::SameLine();
		ImGui::TextUnformatted(magPar == -1 ? "<None>" : minFilterName[magPar]);
		if (ImGui::BeginPopup("mag_filter_popup"))
		{
			ImGui::Text("Mag Filter");
			ImGui::Separator();
			for (int i = 0; i < 2; i++)
				if (ImGui::Selectable(magFilterName[i]))
				{
					magPar = i;
					SetTextureParameters();
				}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Select Wrap.."))
			ImGui::OpenPopup("wrap_popup");
		ImGui::SameLine();
		ImGui::TextUnformatted(wrapPar == -1 ? "<None>" : wrapName[wrapPar]);
		if (ImGui::BeginPopup("wrap_popup"))
		{
			ImGui::Text("Wrap");
			ImGui::Separator();
			for (int i = 0; i < 2; i++)
				if (ImGui::Selectable(wrapName[i]))
				{
					wrapPar = i;
					SetTextureParameters();
				}
			ImGui::EndPopup();
		}*/

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void ModuleRenderExercise::SetTextureParameters()
{
	/*glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter[minPar]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter[magPar]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap[wrapPar]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap[wrapPar]);*/
}