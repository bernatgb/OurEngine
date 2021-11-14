#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleRender.h"
#include "ModuleRenderExercise.h"
#include "ModuleWindow.h"
#include "ModuleTexture.h"
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
	showConsoleWindow = false;
	showTextureWindow = false;

	autoScroll = true;

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
			if (ImGui::MenuItem("Console"))
			{
				showConsoleWindow = !showConsoleWindow;
			}
			if (ImGui::MenuItem("Texture info"))
			{
				showTextureWindow = !showTextureWindow;
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

		ImGui::Text("> Engine name: ");
		ImGui::SameLine();
		ImGui::Text(TITLE);
		ImGui::Text("> Author: miquelmiro3");
		ImGui::Text("> Description: This is MyEngine");
		ImGui::Text("> Libraries: SDL, glew, ImGui, MathGeoLab, DevIL, ...");
		ImGui::Text("> License: MIT License");

		ImGui::Separator();

		ImGui::Text("> Performance:");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}

	if (showTextureWindow)
	{
		App->rendererExercise->DrawTextureImGui(showTextureWindow);
	}

	if (showConsoleWindow)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 200));
		if (ImGui::Begin("Console", &showConsoleWindow))
		{
			if (ImGui::SmallButton("Clear")) 
			{
				for (std::list<char*>::iterator it = App->Items.begin(); it != App->Items.end(); ++it)
					free(*it);
				App->Items.clear();
			}
			ImGui::SameLine();
			ImGui::Checkbox("Auto-scroll", &autoScroll);

			ImGui::Separator();

			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -10), false, ImGuiWindowFlags_HorizontalScrollbar);
			for (std::list<char*>::iterator it = App->Items.begin(); it != App->Items.end(); ++it)
			{
				ImVec4 color;
				bool has_color = false;
				if (strstr((*it), "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
				if (has_color)
					ImGui::PushStyleColor(ImGuiCol_Text, color);
				ImGui::TextUnformatted((*it));
				if (has_color)
					ImGui::PopStyleColor();
			}

			if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(0.0f);

			ImGui::EndChild();

			ImGui::End();
		}
	}

	return UPDATE_CONTINUE;
}

update_status ModuleImGui::PostUpdate()
{
	return UPDATE_CONTINUE;
}
