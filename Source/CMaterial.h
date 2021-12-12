#pragma once
#include "Component.h"

#include "ModuleProgram.h"
#include "Texture.h"

class CMaterial : public Component
{
public:
	CMaterial(bool _enabled, GameObject* _owner, Texture* _texture);
	~CMaterial() override;

	//void Enable() override;
	//void Update() override;
	//void Disable() override;

	void ActivateMaterial();

	void DrawImGui() override;

private:
	Shaders m_Shader;
	Texture* m_Texture;
};