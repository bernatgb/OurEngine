#pragma once
#include "Module.h"

#include "MathGeoLib.h"
#include "Model.h"

class ModuleRenderExercise : public Module
{
public:
	ModuleRenderExercise();
	~ModuleRenderExercise() override;

	bool Init();
	update_status Update();
	bool CleanUp();

	void LoadModel(const char* _fileName);

	void DrawModelImGui();

	Model* modelObj = nullptr;

private:
	unsigned int program;
	unsigned int vBuffer;
	unsigned int texture;
	
	float4x4 model;
};