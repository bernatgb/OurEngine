#pragma once
#include "Globals.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

class GameObject;

enum class ComponentType {
	UNDEFINED,
	TRANSFORM, 
	MESH,
	CAMERA,
	LIGHT
};

class Component
{
public:
	Component(ComponentType _type, bool _enabled, GameObject* _owner)
	{
		m_GUID = rand();
		m_Type = _type;
		m_Enabled = _enabled;
		m_Owner = _owner;

		m_EnableFlag = _enabled; 
		m_DeleteFlag = false;
	};
	virtual ~Component() 
	{
	};

	virtual void Enable() 
	{
	};
	virtual void Update() 
	{
	};
	virtual void Disable() 
	{
	};

	virtual Component* GetAClone(GameObject* _owner)
	{
		return nullptr;
	};
	virtual void NotifyMovement()
	{
	};
	virtual void DrawImGui() 
	{
		// Show enabled variable
		ImGui::Checkbox("Enabled", &m_EnableFlag);

		ImGui::SameLine();

		// Delete button
		if (ImGui::Button("Delete"))
			m_DeleteFlag = true;

		ImGui::Separator();
	};
	
	virtual void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const 
	{
		node.AddMember("Type", rapidjson::Value((int)m_Type), allocator);
		node.AddMember("Id", rapidjson::Value(m_GUID), allocator);
		node.AddMember("Enabled", rapidjson::Value(m_Enabled), allocator);
	};
	virtual void OnLoad(const rapidjson::Value& node) 
	{
		m_Type = (ComponentType)node["Type"].GetInt();
		m_GUID = node["Id"].GetInt();
		m_Enabled = node["Enabled"].GetBool();
	};

	unsigned int m_GUID;
	ComponentType m_Type;
	bool m_Enabled;
	GameObject* m_Owner;

	bool m_EnableFlag;
	bool m_DeleteFlag;
};