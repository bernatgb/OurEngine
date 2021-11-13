#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "SDL/include/SDL.h"

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

	showInfoWindow = false;

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
			if (ImGui::MenuItem("Info"))
			{
				showInfoWindow = !showInfoWindow;
			}
			if (ImGui::MenuItem("Quit"))
			{
				return UPDATE_STOP;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (showInfoWindow)
	{
		ImGui::Begin("About...", &showInfoWindow);
		ImGui::Text(" > Engine name: ");
		ImGui::SameLine();
		ImGui::Text(TITLE);
		ImGui::Text(" > Author: miquelmiro3");
		ImGui::Text(" > Description: This is MyEngine");
		ImGui::Text(" > Libraries: SDL, glew, ImGui, MathGeoLab, ...");
		ImGui::Text(" > License: MIT License");

		if (ImGui::BeginTabBar("Performance")) 
		{
			ImGui::Text(" > Performance:");
			ImGui::Text(" Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleImGui::PostUpdate()
{
	return UPDATE_CONTINUE;
}
