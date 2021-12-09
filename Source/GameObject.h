#pragma once
#include "Globals.h"
#include "Component.h"

#include <vector>

class GameObject
{
public:
	GameObject(const char* _name, GameObject* _parent);
	~GameObject();

	void Start();
	void Update();

	char* m_Name;
	bool m_Active;
	std::vector<Component*> m_Components;
	GameObject* m_Parent;
	std::vector<GameObject*> m_Childern;
};

