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
	program = App->program->CreateProgram(".\\assets\\Shaders\\vertex_shader.vert", ".\\assets\\Shaders\\fragment_shader.frag");
	//program = App->program->CreateProgram(".\\assets\\Shaders\\vertex_shader_phong.vert", ".\\assets\\Shaders\\fragment_shader_phong.frag");

	model = float4x4::FromTRS(float3(2.0f, 0.0f, 0.0f),
		float4x4::RotateZ(pi / 4.0f),
		float3(2.0f, 1.0f, 1.0f));

	MY_LOG("Model: Model creation");

	modelObj = new Model(".\\assets\\Models\\BakerHouse.fbx");

	//FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//FBO texture color
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

	//FBO render buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		MY_LOG("ERROR: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

update_status ModuleRenderExercise::Update()
{
	ImGui::Begin("Scene");

	const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	float x = viewportPanelSize.x, y = viewportPanelSize.y;
	//glViewport(0, 0, x, y);

	//App->renderer->Resize(x, y);

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

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);


	/*unsigned int loc;

	float3 light_pos = float3(5.0f, 5.0f, 5.0f);
	loc = glGetUniformLocation(program, "light_pos");
	if (loc < 0) MY_LOG("Uniform location not found: light_pos");
	glUniform3fv(loc, 1, &light_pos[0]);

	loc = glGetUniformLocation(program, "cam_pos");
	if (loc < 0) MY_LOG("Uniform location not found: cam_pos");
	glUniform3fv(loc, 1, &App->camera->eye[0]);
	
	float3 color_a = float3(0.25f, 0.25f, 0.25f);
	loc = glGetUniformLocation(program, "color_a");
	if (loc < 0) MY_LOG("Uniform location not found: color_a");
	glUniform3fv(loc, 1, &color_a[0]);

	float3 color_l = float3(1.0f, 1.0f, 1.0f);
	loc = glGetUniformLocation(program, "color_l");
	if (loc < 0) MY_LOG("Uniform location not found: color_l");
	glUniform3fv(loc, 1, &color_l[0]);


	float kd = 1.0f;
	loc = glGetUniformLocation(program, "kd");
	if (loc < 0) MY_LOG("Uniform location not found: kd");
	glUniform1f(loc, kd);

	float ks = 0.0f;
	loc = glGetUniformLocation(program, "ks");
	if (loc < 0) MY_LOG("Uniform location not found: ks");
	glUniform1f(loc, ks);

	float n = 0.0f;
	loc = glGetUniformLocation(program, "n");
	if (loc < 0) MY_LOG("Uniform location not found: n");
	glUniform1f(loc, n);*/

	//modelObj->Draw(program);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGui::Image((void*)fbo_texture, ImVec2{ x, y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	ImGui::End();

	return UPDATE_CONTINUE;
}

bool ModuleRenderExercise::CleanUp()
{
	//glDeleteBuffers(1, &vBuffer);

	delete modelObj;

	glDeleteProgram(program);

	//glDeleteTextures(1, &texture);

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &fbo_texture);
	glDeleteRenderbuffers(1, &rbo);

	return true;
}

void ModuleRenderExercise::DrawModelImGui()
{
	/*ImGui::Begin("Scene Window");

	ImVec2 pos = ImGui::GetCursorScreenPos();

	ImGui::GetWindowDrawList()->AddImage(
		(void*)fbo_texture, ImVec2(ImGui::GetCursorScreenPos()),
		ImVec2(ImGui::GetCursorScreenPos().x + window.getWidth() / 2, ImGui::GetCursorScreenPos().y + window.getHeight() / 2), ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();*/

	modelObj->DrawImGui();
}

void ModuleRenderExercise::LoadModel(const char* _fileName)
{
	delete modelObj;
	modelObj = new Model(_fileName);

	//App->camera->AdjustToModel(modelObj);
}