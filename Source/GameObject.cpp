#include "GameObject.h"

#include "Application.h"
#include "ModuleRender.h"
#include "GL/glew.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

GameObject::GameObject(const char* _name, GameObject* _parent)
{
	m_Name = new char[100];
	strcpy(m_Name, _name);

	m_Active = true;
	m_Selected = false;
	m_Parent = _parent;

	m_Transform = new CTransform(true, this);
	m_Material = nullptr;
}

GameObject::~GameObject()
{
	delete[] m_Name;

	delete m_Transform;
	delete m_Material;

	for (unsigned int i = 0; i < m_Components.size(); ++i)
		delete m_Components[i];
	
	for (unsigned int i = 0; i < m_Children.size(); ++i)
		delete m_Children[i];
}

void GameObject::Start()
{
	if (m_Parent != nullptr) 
		m_Transform->ParentTransformUpdate(m_Parent->m_Transform->m_AccumulativeModelMatrix);

	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		if (m_Components[i]->m_Enabled)
			m_Components[i]->Enable();
	}
}

void GameObject::Update()
{
	if (!m_Active)
		return;

	if (m_Material != nullptr) 
	{
		m_Material->ActivateMaterial();
	}

	glUniformMatrix4fv(glGetUniformLocation(App->renderer->program, "model"), 1, GL_TRUE, &m_Transform->m_AccumulativeModelMatrix[0][0]);

	for (unsigned int i = 0; i < m_Components.size(); ++i)
		m_Components[i]->Update();

	for (unsigned int i = 0; i < m_Children.size(); ++i)
		m_Children[i]->Update();
}

void GameObject::SetMaterial(Texture* _texture)
{
	m_Material = new CMaterial(true, this, _texture);
}

GameObject* GameObject::AddChild(const char* _name)
{
	GameObject* child = new GameObject(_name, this);
	m_Children.push_back(child);

	return child;
}

void GameObject::DeleteChild(GameObject* _go)
{
	bool found = false;
	for (unsigned int i = 0; i < m_Children.size(); ++i) 
	{
		if (found)
		{
			m_Children[i-1] = m_Children[i];
		}
		else if (m_Children[i] == _go)
		{
			delete m_Children[i];
			found = true;
		}
	}

	if (found) 
	{
		m_Children.pop_back();
	}
}

void GameObject::DrawImGui()
{
	ImGui::InputText("Name", m_Name, 100);
	ImGui::Checkbox("Active", &m_Active);
	//header

	m_Transform->DrawImGui();

	for (unsigned int i = 0; i < m_Components.size(); ++i) 
		m_Components[i]->DrawImGui();

	if (m_Material != nullptr)
		m_Material->DrawImGui();
}

template<typename T>
T* GameObject::GetComponent()
{
	ComponentType type;
	switch (T)
	{
	case CTransform:
		type = ComponentType::TRANSFORM;
		break;
	case CMesh:
		type = ComponentType::MESH;
		break;
	default:
		return nullptr;
	}

	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		if (m_Components[i]->m_Type == type)
			return (T)m_Components[i];
	}

	return nullptr;
}

void GameObject::AddComponent(Component* _newComponent)
{
	m_Components.push_back(_newComponent);
}

/*template<typename T>
void GameObject::AddComponent(T* _newComponent)
{
	m_Components.push_back(_newComponent);
}*/

/*template CTransform* GameObject::GetComponent<CTransform>();
template CMaterial* GameObject::GetComponent<CMaterial>();
template CMesh* GameObject::GetComponent<CMesh>();

template void GameObject::AddComponent<CMesh>(CMesh*);*/