#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleRender.h"
#include "ModuleRenderExercise.h"
#include "ModuleWindow.h"
#include "ModuleTexture.h"
#include "ModuleCamera.h"
#include "SDL/include/SDL.h"
#include "GL/glew.h"

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

	for (std::list<char*>::iterator it = Items.begin(); it != Items.end(); ++it)
	{
		delete[] * it;
	}
}

bool ModuleImGui::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init();

	ImGui::StyleColorsDark();

	inspector = false;
	hierarchy = false;
	config = false;
	console = false;

	showInfoWindow = false;
	showConsoleWindow = false;
	showCameraWindow = false;
	showTimeWindow = true;

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
			if (ImGui::MenuItem("About"))
				showInfoWindow = !showInfoWindow;
			if (ImGui::MenuItem("Quit"))
				return UPDATE_STOP;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Console"))
				showConsoleWindow = !showConsoleWindow;
			if (ImGui::MenuItem("Camera"))
				showCameraWindow = !showCameraWindow;
			if (ImGui::MenuItem("Model info"))
				showModelWindow = !showModelWindow;
			if (ImGui::MenuItem("Time"))
				showTimeWindow = !showTimeWindow;
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	inspector = false;
	hierarchy = false;
	config = false;
	console = false;

	//ImageButton()

	if (showInfoWindow)
	{
		if (ImGui::Begin("About...", &showInfoWindow)) 
		{
			About();
			ImGui::End();
		}
	}

	if (showCameraWindow)
	{
		if (ImGui::Begin("Camera", &showCameraWindow))
		{
			App->camera->DrawImGui();
			ImGui::End();
		}
	}

	if (showModelWindow)
	{
		if (ImGui::Begin("Model", &showModelWindow))
		{
			App->rendererExercise->DrawModelImGui();
			ImGui::End();
		}
	}

	if (showTimeWindow)
	{
		if (ImGui::Begin("Time", &showTimeWindow))
		{
			Time::DrawImGui();
			ImGui::End();
		}
	}

	if (showConsoleWindow)
		Console(showConsoleWindow);

	return UPDATE_CONTINUE;
}

update_status ModuleImGui::PostUpdate()
{
	return UPDATE_CONTINUE;
}

void ModuleImGui::About()
{
	ImGui::BulletText("Engine name: ");
	ImGui::SameLine();
	ImGui::Text(TITLE);
	ImGui::BulletText("Author: miquelmiro3");
	ImGui::BulletText("Description: This is MyEngine");
	ImGui::BulletText("Libraries: SDL, glew, ImGui, MathGeoLab, DevIL, Assimp, ...");
	ImGui::BulletText("License: MIT License");
	if (ImGui::Button("Github"))
		ShellExecute(0, 0, "https://github.com/miquelmiro3/MyEngine", 0, 0, SW_SHOW);
	ImGui::SameLine();
	ImGui::Text("https://github.com/miquelmiro3/MyEngine");

	ImGui::Separator();

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::Separator();

	SDL_version version;
	SDL_GetVersion(&version);
	int vram_free, vram_budget;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vram_free);
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vram_budget);
	float vram_budget_mb = vram_budget / 1024.0f;
	float vram_free_mb = vram_free / 1024.0f;
	float vram_usage_mb = vram_budget_mb - vram_free_mb;

	ImGui::Text("SDL Version: %d.%d.%d", version.major, version.minor, version.patch);
	ImGui::Text("CPUs: %d", SDL_GetCPUCount());
	ImGui::Text("System RAM: %.1f Gb", SDL_GetSystemRAM() / 1024.0f);
	ImGui::Text("GPU: %s", (unsigned char*)glGetString(GL_RENDERER));
	ImGui::Text("Brand: %s", (unsigned char*)glGetString(GL_VENDOR));
	ImGui::Text("VRAM Budget: %.1f Mb", vram_budget_mb);
	ImGui::Text("Vram Usage: %.1f Mb", vram_usage_mb);
	ImGui::Text("Vram Avaliable: %.1f Mb", vram_free_mb);
}

void ModuleImGui::Console(bool& show)
{
	if (ImGui::Begin("Console", &show))
	{
		if (ImGui::SmallButton("Clear"))
		{
			for (std::list<char*>::iterator it = Items.begin(); it != Items.end(); ++it)
				free(*it);
			Items.clear();
		}
		ImGui::SameLine();
		ImGui::Checkbox("Auto-scroll", &autoScroll);

		ImGui::Separator();

		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -10), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (std::list<char*>::iterator it = Items.begin(); it != Items.end(); ++it)
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
