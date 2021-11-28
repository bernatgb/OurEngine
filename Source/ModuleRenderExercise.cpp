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
}

ModuleRenderExercise::~ModuleRenderExercise()
{
}

bool ModuleRenderExercise::Init()
{
	MY_LOG("Buffer: Creating vertex buffer");
	/*GLfloat vertex[] = {
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
	App->texture->LoadTextureData("Lenna.png");
	glGenerateTextureMipmap(texture);*/

	MY_LOG("Shaders: Creating program");
	program = App->program->CreateProgram("..\\Source\\shaders\\vertex_shader.vert", "..\\Source\\shaders\\fragment_shader.frag");
	//program = App->program->CreateProgram("..\\Source\\shaders\\vertex_shader_phong.vert", "..\\Source\\shaders\\fragment_shader_phong.frag");

	model = float4x4::FromTRS(float3(2.0f, 0.0f, 0.0f),
		float4x4::RotateZ(pi / 4.0f),
		float3(2.0f, 1.0f, 1.0f));

	MY_LOG("Model: Model creation");

	modelObj = new Model("BakerHouse.fbx");

	return true;
}

update_status ModuleRenderExercise::Update()
{
	/*glGenBuffers(1, &vBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);

	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(program, "texture"), 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//glDisableVertexAttribArray(0);
	}*/

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);

	/*float3 light_pos = float3(5.0f, 5.0f, 5.0f);
	glUniform3fv(glGetUniformLocation(program, "light_pos"), 1, &light_pos[0]);
	glUniform3fv(glGetUniformLocation(program, "cam_pos"), 1, &App->camera->eye[0]);
	
	float3 color_a = float3(0.25f, 0.25f, 0.25f);
	glUniform3fv(glGetUniformLocation(program, "color_a"), 1, &color_a[0]);
	float3 color_l = float3(1.0f, 1.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(program, "color_l"), 1, &color_l[0]);

	float kd = 0.75f;
	glUniform1f(glGetUniformLocation(program, "kd"), kd);
	float ks = 0.0f;
	glUniform1f(glGetUniformLocation(program, "ks"), ks);
	float n = 0.0f;
	glUniform1f(glGetUniformLocation(program, "n"), n);*/

	modelObj->Draw(program);

	return UPDATE_CONTINUE;
}

bool ModuleRenderExercise::CleanUp()
{
	//glDeleteBuffers(1, &vBuffer);

	delete modelObj;

	glDeleteProgram(program);

	//glDeleteTextures(1, &texture);

	return true;
}

void ModuleRenderExercise::DrawModelImGui()
{
	modelObj->DrawImGui();
}

void ModuleRenderExercise::LoadModel(const char* _fileName)
{
	delete modelObj;
	modelObj = new Model(_fileName);

	App->camera->AdjustToModel(modelObj);
}