#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"
#include "ModuleDebugDraw.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "Config.h"
#include "CubeMap.h"
#include "SDL.h"
#include "GL/glew.h"
#include <MathGeoLib.h>

#include "Material.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char* tmp_source = "", * tmp_type = "", * tmp_severity = "";
	switch (source) {
	case GL_DEBUG_SOURCE_API: tmp_source = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: tmp_source = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: tmp_source = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: tmp_source = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: tmp_source = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: tmp_source = "Other"; break;
	};
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: tmp_type = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tmp_type = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tmp_type = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: tmp_type = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: tmp_type = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER: tmp_type = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: tmp_type = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: tmp_type = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: tmp_type = "Other"; break;
	};
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: tmp_severity = "high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: tmp_severity = "medium"; break;
	case GL_DEBUG_SEVERITY_LOW: tmp_severity = "low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: tmp_severity = "notification"; break;
	};
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
		MY_LOG("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n", tmp_source, tmp_type, tmp_severity, id, message);
}

ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
}

/*float4x4 ViewMatrix(float3 target, float3 eye)
{
	float3 forward = target - eye;
	forward.Normalize();
	float3 right = Cross(forward, float3::unitY);
	right.Normalize();
	float3 up = Cross(right, forward);
	up.Normalize();

	float4x4 rotationMatrix = {
		{right.x, up.x, -forward.x, 0},
		{right.y, up.y, -forward.y, 0},
		{right.z, up.z, -forward.z, 0},
		{0, 0, 0, 1}
	};

	float4x4 translationMatrix = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{-eye.x, -eye.y, -eye.z, 1}
	};

	return rotationMatrix * translationMatrix;
}*/

// Called before render is available
bool ModuleRender::Init()
{
	MY_LOG("Creating Renderer context");

	//glewInit();
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // stencil buffer with 8 bits

	context = SDL_GL_CreateContext(App->window->GetWindow());

	GLenum err = glewInit();
	// … check for errors
	MY_LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	MY_LOG("Vendor: %s", glGetString(GL_VENDOR));
	MY_LOG("Renderer: %s", glGetString(GL_RENDERER));
	MY_LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	MY_LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEPTH_TEST); // Enable depth test
	glEnable(GL_CULL_FACE); // Enable cull backward faces
	glFrontFace(GL_CCW); // Front faces will be counter clockwise

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ErrorDebugging
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Framebuffer

	MY_LOG("Shaders: Creating program");
	program = App->program->CreateProgram(".\\assets\\Shaders\\phongBRDFengine_SimpleVS.glsl", ".\\assets\\Shaders\\phongBRDFengine_SimplePS.glsl");
	//program = App->program->CreateProgram(".\\assets\\Shaders\\phongBRDFandNormalMap_VS.glsl", ".\\assets\\Shaders\\phongBRDFandNormalMap_PS.glsl");

	MY_LOG("Framebuffer: Creating framebuffer");
	//FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//FBO texture color
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//FBO render buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		MY_LOG("ERROR: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Light buffer
	glGenBuffers(1, &m_lightsUBO); // this generates UBO, OK
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightsUBO); // this binds it, OK
	glBufferData(GL_UNIFORM_BUFFER, 16 + 80 * 20, &m_LightsContainer, GL_DYNAMIC_DRAW); // this allocates space for the UBO. 
	glBindBufferRange(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(program, "Lights"), m_lightsUBO, 0, 16 + 80 * 20); // this binds UBO to Buffer Index
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//Create cube map
	std::vector<std::string> faces
	{
		".\\assets\\Textures\\Skybox\\right.jpg",
		".\\assets\\Textures\\Skybox\\left.jpg",
		".\\assets\\Textures\\Skybox\\top.jpg",
		".\\assets\\Textures\\Skybox\\bottom.jpg",
		".\\assets\\Textures\\Skybox\\front.jpg",
		".\\assets\\Textures\\Skybox\\back.jpg"
	};

	cubeMap = new CubeMap();
	cubeMap->LoadTexture(faces);
	m_ActiveCubeMap = 0;

	viewportPanelSize = float2(0.0f, 0.0f);

	return true;
}

update_status ModuleRender::PreUpdate()
{
	OPTICK_CATEGORY("ModuleRender::PreUpdate", Optick::Category::Rendering);

	int w;
	int h;
	SDL_GetWindowSize(App->window->GetWindow(), &w, &h);
	glViewport(0, 0, w, h);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	App->scene->m_Lights.clear();
	App->scene->RecursiveSearch(App->scene->GetRoot(), true, false);
	App->scene->GetQuadtree()->SetObejctsInFrustum(App->camera->GetCullingCamera());

	m_LightsContainer.numberOfLights = App->scene->m_Lights.size();
	int i = 0;
	for (auto it = App->scene->m_Lights.cbegin(); it != App->scene->m_Lights.cend(); ++it) {
		m_LightsContainer.lights[i] = *it;
		++i;
	}

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	OPTICK_CATEGORY("ModuleRender::Update", Optick::Category::Rendering);

	ImGui::Begin("Scene");

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const ImVec2 newViewportPanelSize = ImGui::GetContentRegionAvail();
	viewportSizeChanged = false;
	if (viewportPanelSize.x != newViewportPanelSize.x || viewportPanelSize.y != newViewportPanelSize.y) {
		viewportPanelSize.x = newViewportPanelSize.x;
		viewportPanelSize.y = newViewportPanelSize.y;
		viewportSizeChanged = true;

		//Resize textures
		glBindTexture(GL_TEXTURE_2D, fbo_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportPanelSize.x, viewportPanelSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewportPanelSize.x, viewportPanelSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		
		App->camera->WindowResized(viewportPanelSize.x, viewportPanelSize.y);
	}

	glViewport(0, 0, viewportPanelSize.x, viewportPanelSize.y);

	sceneWindowPos = float2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &App->camera->GetView()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &App->camera->GetProj()[0][0]);

	unsigned int loc;

	loc = glGetUniformLocation(program, "camPos");
	if (loc < 0) MY_LOG("Uniform location not found: camPos");
	float3 camPos = float3(App->camera->GetFrustum()->Pos()[0], App->camera->GetFrustum()->Pos()[1], App->camera->GetFrustum()->Pos()[2]);
	glUniform3fv(loc, 1, &camPos[0]);

	loc = glGetUniformLocation(program, "ambientColor");
	if (loc < 0) MY_LOG("Uniform location not found: ambientColor");
	glUniform3fv(loc, 1, &m_AmbientColor[0]);

	// TODO: ACTIVATE ALL LIGHTS AND THINGS
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 + 80 * 20, &m_LightsContainer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	App->scene->Draw(program);
	App->debugDraw->Draw(App->camera->GetView(), App->camera->GetProj(), viewportPanelSize.x, viewportPanelSize.y);

	// Draw cubemap
	if (cubeMap != nullptr)
		cubeMap->Draw(m_ActiveCubeMap);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGui::Image((void*)fbo_texture, ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	ImGui::End();

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	OPTICK_CATEGORY("ModuleRender::PostUpdate", Optick::Category::Rendering);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(App->window->GetWindow());

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	MY_LOG("Destroying renderer");

	//Destroy program
	glDeleteProgram(program);

	//Destroy framebuffer
	glDeleteRenderbuffers(1, &rbo);
	glDeleteTextures(1, &fbo_texture);
	glDeleteFramebuffers(1, &fbo);

	//Destroy cubemap
	delete cubeMap;

	//Destroy window
	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
	glViewport(0, 0, width, height);
}

void ModuleRender::DrawImGui()
{
	if (ImGui::CollapsingHeader("Renderer")) 
	{
		ImGui::ColorEdit3("Ambient color", &m_AmbientColor[0]);

		ImGui::Checkbox("Draw axis", &Config::m_DrawAxis);
		ImGui::Checkbox("Draw grid", &Config::m_DrawGrid);
	}
}

void ModuleRender::ActivateMaterial(Material* _material)
{
	unsigned int loc;

	loc = glGetUniformLocation(program, "hasDiffuseTex");
	if (loc < 0) MY_LOG("Uniform location not found: hasDiffuseTex");
	glUniform1i(loc, _material->m_DiffuseTexture != nullptr);

	loc = glGetUniformLocation(program, "hasSpecularTex");
	if (loc < 0) MY_LOG("Uniform location not found: hasSpecularTex");
	glUniform1i(loc, _material->m_SpecularTexture != nullptr);

	loc = glGetUniformLocation(program, "shininessAlpha");
	if (loc < 0) MY_LOG("Uniform location not found: shininessAlpha");
	glUniform1i(loc, _material->m_ShininessAlpha);

	//loc = glGetUniformLocation(program, "hasNormalMap");
	//if (loc < 0) MY_LOG("Uniform location not found: hasNormalMap");
	//glUniform1i(loc, _material->m_NormalMap != nullptr);

	loc = glGetUniformLocation(program, "shininess");
	if (loc < 0) MY_LOG("Uniform location not found: shininess");
	glUniform1f(loc, _material->m_Shininess);

	loc = glGetUniformLocation(program, "diffuseColor");
	if (loc < 0) MY_LOG("Uniform location not found: diffuseColor");
	glUniform3fv(loc, 1, &_material->m_DiffuseColor[0]);

	loc = glGetUniformLocation(program, "specularColor");
	if (loc < 0) MY_LOG("Uniform location not found: specularColor");
	glUniform3fv(loc, 1, &_material->m_SpecularColor[0]);

	if (_material->m_DiffuseTexture != nullptr)
	{
		loc = glGetUniformLocation(program, "diffuseTex");
		if (loc < 0) MY_LOG("Uniform location not found: diffuseTex");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _material->m_DiffuseTexture->m_Texture);
		glUniform1i(loc, 0);
	}
	
	if (_material->m_SpecularTexture != nullptr)
	{
		loc = glGetUniformLocation(program, "specularTex");
		if (loc < 0) MY_LOG("Uniform location not found: specularTex");

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _material->m_SpecularTexture->m_Texture);
		glUniform1i(loc, 0);
	}
	/*
	if (_material->m_NormalMap != nullptr)
	{
		loc = glGetUniformLocation(program, "normalMap");
		if (loc < 0) MY_LOG("Uniform location not found: normalMap");

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _material->m_NormalMap->m_Texture);
		glUniform1i(loc, 0);
	}*/
}

CubeMap* ModuleRender::GetCubeMap()
{
	return cubeMap;
}
