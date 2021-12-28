#pragma once
#include "Globals.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class GameObject;

enum class ComponentType {
	UNDEFINED,
	TRANSFORM, 
	MESH,
	MATERIAL,
	CAMERA
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

	virtual void NotifyMovement()
	{
	};
	virtual void DrawImGui() 
	{
	};
	
	virtual void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const {};
	virtual void OnLoad(const rapidjson::Value& node) {};

	unsigned int m_GUID;
	ComponentType m_Type;
	bool m_Enabled;
	GameObject* m_Owner;
};