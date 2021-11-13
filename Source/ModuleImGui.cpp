#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

ModuleImGui::ModuleImGui()
{
}

ModuleImGui::~ModuleImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

bool ModuleImGui::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init();

	ImGui::StyleColorsDark();

	showAboutWindow = false;

	return true;
}

update_status ModuleImGui::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleImGui::Update()
{
	//ImGui::ShowDemoWindow();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load file"))
			{
				//Do something
			}
			if (ImGui::MenuItem("About"))
			{
				showAboutWindow = !showAboutWindow;
			}
			if (ImGui::MenuItem("Quit"))
			{
				return UPDATE_STOP;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (showAboutWindow) 
	{
		ImGui::Begin("About...");
		ImGui::Text(" > Engine name: ");
		ImGui::SameLine();
		ImGui::Text(TITLE);
		ImGui::Text(" > Author: miquelmiro3");
		ImGui::Text(" > Description:");
		ImGui::Text(" > Libraries:");
		ImGui::Text(" > License:");
		ImGui::End();
	}

	// Should be in the post but it cant go after the post of the renderer
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

update_status ModuleImGui::PostUpdate()
{
	return UPDATE_CONTINUE;
}
