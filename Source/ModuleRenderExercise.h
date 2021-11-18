#pragma once
#include "Module.h"

#include "MathGeoLib.h"
#include "Model.h"

class ModuleRenderExercise : public Module
{
public:
	ModuleRenderExercise();
	~ModuleRenderExercise();

	bool Init();
	update_status Update();
	bool CleanUp();

	void DrawTextureImGui(bool& showWindow);


private:
	unsigned int program;
	unsigned int vBuffer;
	unsigned int texture;
	
	float4x4 model;

	Model* modelObj;

	void SetTextureParameters();
	/*unsigned int minPar;
	unsigned int magPar;
	unsigned int wrapPar;

	unsigned int* minFilter;
	unsigned int* magFilter;
	unsigned int* wrap;
	
	std::vector<string> minFilterName{
		"GL_NEAREST", "GL_LINEAR",
		"GL_NEAREST_MIPMAP_NEAREST", "GL_LINEAR_MIPMAP_NEAREST",
		"GL_NEAREST_MIPMAP_LINEAR", "GL_LINEAR_MIPMAP_LINEAR"
	};
	std::vector<string> magFilterName{
		"GL_NEAREST", "GL_LINEAR"
	};
	std::vector<string> wrapName {
		"GL_CLAMP", "GL_REPEAT"
	};*/
};