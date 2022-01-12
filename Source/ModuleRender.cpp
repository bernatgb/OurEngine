#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"
#include "ModuleDebugDraw.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "CubeMap.h"
#include "SDL.h"
#include "GL/glew.h"
#include <MathGeoLib.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"

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

	context = SDL_GL_CreateContext(App->window->window);

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
	//program = App->program->CreateProgram(".\\assets\\Shaders\\vertex_shader.vert", ".\\assets\\Shaders\\fragment_shader.frag");
	//program = App->program->CreateProgram(".\\assets\\Shaders\\vertex_shader_phong.vert", ".\\assets\\Shaders\\fragment_shader_phong.frag");
	program = App->program->CreateProgram(".\\assets\\Shaders\\vertex_shader_phongBRDF.vert", ".\\assets\\Shaders\\fragment_shader_phongBRDF.frag");

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

	viewportPanelSize = float2(0.0f, 0.0f);

	return true;
}

update_status ModuleRender::PreUpdate()
{
	int w;
	int h;
	SDL_GetWindowSize(App->window->window, &w, &h);
	glViewport(0, 0, w, h);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	ImGui::Begin("Scene");

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const ImVec2 newViewportPanelSize = ImGui::GetContentRegionAvail();
	if (viewportPanelSize.x != newViewportPanelSize.x || viewportPanelSize.y != newViewportPanelSize.y) {
		viewportPanelSize.x = newViewportPanelSize.x;
		viewportPanelSize.y = newViewportPanelSize.y;

		//Resize textures
		glBindTexture(GL_TEXTURE_2D, fbo_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportPanelSize.x, viewportPanelSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewportPanelSize.x, viewportPanelSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		
		App->camera->WindowResized(viewportPanelSize.x, viewportPanelSize.y);
	}


	// Gizmos
	if (App->scene->GetSelectedGO() != nullptr) 
	{
		ImGuizmo::OPERATION currentGizmoOperation(ImGuizmo::TRANSLATE);
		if (App->input->GetKey(SDL_SCANCODE_T))
			currentGizmoOperation = ImGuizmo::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_R))
			currentGizmoOperation = ImGuizmo::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_Y))
			currentGizmoOperation = ImGuizmo::SCALE;

		float4x4 mat4 = App->scene->GetSelectedGO()->m_Transform->m_AccumulativeModelMatrix.Transposed();
		float4x4 view = App->camera->view.Transposed();
		float4x4 proj = App->camera->proj.Transposed();

		const ImVec2 newViewportPosition = ImGui::GetWindowPos();
		const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		const ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		ImGuizmo::SetRect(newViewportPosition.x + vMin.x, newViewportPosition.y + vMin.y, viewportPanelSize.x, viewportPanelSize.y);
		if (ImGuizmo::Manipulate(&view[0][0], &proj[0][0], currentGizmoOperation, ImGuizmo::LOCAL, &mat4[0][0], NULL, NULL))
			App->scene->GetSelectedGO()->m_Transform->GizmoTransformChange(mat4.Transposed());
	}
	

	glViewport(0, 0, viewportPanelSize.x, viewportPanelSize.y);
	/*
	ImGui::Text("x=%f, y=%f", viewportPanelSize.x, viewportPanelSize.y);
	int mx, my;
	ImVec2 v2 = ImGui::GetMousePos();
	ImGui::Text("mx = %f, my = %f", v2.x, v2.y);
	ImGuiViewport* v1 = ImGui::GetMainViewport();
	ImVec2 v = v1->GetCenter();
	//v = v1->GetWorkCenter();
	ImGui::Text("vx = %f, vy = %f", v.x, v.y);
	ImGuiViewport* vp = ImGui::GetWindowViewport();
	ImVec2 vpp = ImGui::GetWindowPos(); 
	ImVec2 vpi = vpp;
	//vpp = vp->DrawData->DisplaySize;
	ImGui::Text("vpx = %f, vpy = %f", vpp.x, vpp.y);
	//ImGui::Text("dx = %f, dy = %f", dd.DisplaySize.x, dd.DisplaySize.y);

	ImGui::Text("rx = %f, ry = %f", viewportPanelSize.x / 2 + vpi.x, viewportPanelSize.y / 2 + vpi.y);
	*/
	sceneWindowPos = float2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);
	// Model?
	//GLfloat camPos = App->camera->GetFrustum()->Pos()[0];
	//const GLfloat* constCamPos = &camPos;
	//glUniform3fv(glGetUniformLocation(program, "cameraPosition"), 1, constCamPos);


	unsigned int loc;
	/*
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
	glUniform1f(loc, n);
	*/
	float3 lightDirection = float3(0.0, -1.0, 1.0);
	loc = glGetUniformLocation(program, "lightDirection");
	if (loc < 0) MY_LOG("Uniform location not found: lightDirection");
	glUniform3fv(loc, 1, &lightDirection[0]);

	float3 ambientColor = float3(0.01, 0.01, 0.01);
	loc = glGetUniformLocation(program, "ambientColor");
	if (loc < 0) MY_LOG("Uniform location not found: ambientColor");
	glUniform3fv(loc, 1, &ambientColor[0]);

	float3 specularColor = float3(0.08, 0.08, 0.08);
	loc = glGetUniformLocation(program, "specularColor");
	if (loc < 0) MY_LOG("Uniform location not found: specularColor");
	glUniform3fv(loc, 1, &specularColor[0]);

	float shininess = 300.0;
	loc = glGetUniformLocation(program, "shininess");
	if (loc < 0) MY_LOG("Uniform location not found: shininess");
	glUniform1f(loc, shininess);

	float3 lightColor = float3(1.0, 1.0, 1.0);
	loc = glGetUniformLocation(program, "lightColor");
	if (loc < 0) MY_LOG("Uniform location not found: lightColor");
	glUniform3fv(loc, 1, &lightColor[0]);


	App->scene->Draw(program);
	App->debugDraw->Draw(App->camera->view, App->camera->proj, viewportPanelSize.x, viewportPanelSize.y);

	// Draw cubemap
	cubeMap->Draw(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*
	//Selection highlighting
	glEnable(GL_DEPTH_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glStencilMask(0x00); // make sure we don't update the stencil buffer while drawing the floor
	//normalShader.use();
	//DrawFloor()

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	//DrawTwoContainers();

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	unsigned singleColorProgram = App->program->CreateProgram(".\\assets\\Shaders\\vertex_shader.vert", ".\\assets\\Shaders\\fragment_shader_single_color.frag");
	//".\\assets\\Shaders\\fragment_shader_single_color.frag".use();
	//DrawTwoScaledUpContainers();
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);
	// End selection highlighting */

	ImGui::Image((void*)fbo_texture, ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	ImGui::End();

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(App->window->window);

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

CubeMap* ModuleRender::GetCubeMap()
{
	return cubeMap;
}


/*
void ModuleImGui::EditTransform() // (const Camera& camera, matrix_t& matrix)
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];

	float4x4 matrix = float4x4::identity;

	ImGuizmo::DecomposeMatrixToComponents(&matrix[0][0], matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, "%.3f");
	ImGui::InputFloat3("Rt", matrixRotation, "%.3f");
	ImGui::InputFloat3("Sc", matrixScale, "%.3f");
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &matrix[0][0]);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();
	vec snap; // WTF is this?
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		//snap = config.mSnapTranslation;
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImGuizmo::ROTATE:
		//snap = config.mSnapRotation;
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImGuizmo::SCALE:
		//snap = config.mSnapScale;
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(&App->camera->view[0][0], &App->camera->proj[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &matrix[0][0], NULL, useSnap ? &snap.x : NULL);
}
*/