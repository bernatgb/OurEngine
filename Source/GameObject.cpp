#include "GameObject.h"

GameObject::GameObject(const char* _name, GameObject* _parent)
{
	m_Name = new char[strlen(_name) + 1];
	strcpy(m_Name, _name);

	m_Active = true;
	m_Parent = _parent;
}

GameObject::~GameObject()
{
}

void GameObject::Start()
{
}

void GameObject::Update()
{
}
