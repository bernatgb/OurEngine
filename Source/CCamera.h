#pragma once
#include "Component.h"

class CCamera : public Component
{
public:
	CCamera(bool _enabled, GameObject* _owner);
	~CCamera() override;

	void Enable() override;
	void Update() override;
	void Disable() override;

	void NotifyMovement() override;
	void DrawImGui() override;

	bool m_CurrentCamera;
};