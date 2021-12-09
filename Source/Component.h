#pragma once
#include "Globals.h"

class GameObject;

enum ComponentType {
	UNDEFINED,
	TRANSFORM, 
	MESH,
	MATERIAL
};

class Component
{
public:
	Component()
	{
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
	
	ComponentType m_Type;
	bool m_Enabled;
	GameObject* m_Owner;
};