#include "CTransform.h"

#include "GameObject.h"

#include "SceneImporter.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

CTransform::CTransform(bool _enabled, GameObject* _owner) : Component(ComponentType::TRANSFORM, _enabled, _owner)
{
	m_AccumulativeModelMatrix = float4x4::identity;

	m_ModelMatrix = float4x4::identity;
	m_Position = float3::zero;
	m_RotationEuler = float3::zero;
	m_Scale = float3::one;
	m_Rotation = Quat::identity;
}

CTransform::~CTransform()
{
}

void CTransform::NotifyMovement()
{
	//Change AccumulativeModelMatrix
	if (m_Owner->m_Parent != nullptr)
		m_AccumulativeModelMatrix = m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * m_ModelMatrix;
	else
		m_AccumulativeModelMatrix = m_ModelMatrix;

	//TODO: NOTIFY GO OWNER

	//Notify other components
	for (unsigned int i = 0; i < m_Owner->m_Components.size(); ++i)
		m_Owner->m_Components[i]->NotifyMovement();

	//Notify owner children
	for (unsigned int i = 0; i < m_Owner->m_Children.size(); ++i)
		m_Owner->m_Children[i]->m_Transform->NotifyMovement();
}

void CTransform::DrawImGui()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Position", &m_Position[0], 0.5f))
		{
			RecalculateModelMatrix();
		}
		if (ImGui::DragFloat3("Rotation", &m_RotationEuler[0], 10.0f, 0.0f, 360.0f))
		{
			//maybe use the delta rotation deltaRotation (so the quat isnt recalculated)
			m_Rotation = Quat::FromEulerXYZ(m_RotationEuler.x * DEGTORAD, m_RotationEuler.y * DEGTORAD, m_RotationEuler.z * DEGTORAD);
			RecalculateModelMatrix();
		}
		if (ImGui::DragFloat3("Scale", &m_Scale[0], 0.5f))
		{
			RecalculateModelMatrix();
		}
	}
}

void CTransform::OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const
{
	Component::OnSave(node, allocator);

	node.AddMember("Position", importer::Float3ToValue(m_Position, allocator), allocator);
	node.AddMember("Rotation", importer::Float3ToValue(m_RotationEuler, allocator), allocator);
	node.AddMember("Scale", importer::Float3ToValue(m_Scale, allocator), allocator);
}

void CTransform::OnLoad(const rapidjson::Value& node)
{
	Component::OnLoad(node);

	m_Position = importer::ValueToFloat3(node["Position"]);
	m_RotationEuler = importer::ValueToFloat3(node["Rotation"]);
	m_Scale = importer::ValueToFloat3(node["Scale"]);

	m_Rotation = Quat::FromEulerXYZ(m_RotationEuler.x * DEGTORAD, m_RotationEuler.y * DEGTORAD, m_RotationEuler.z * DEGTORAD);

	RecalculateModelMatrix();
}

void CTransform::RecalculateModelMatrix()
{
	m_ModelMatrix = float4x4::FromTRS(m_Position, m_Rotation, m_Scale);

	NotifyMovement();
}


float3 CTransform::GetPos() const
{
	if (m_Owner->m_Parent != nullptr)
		return (m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * m_Position.ToPos4()).Float3Part();
	return m_Position;
}

float3 CTransform::GetForward() const
{
	if (m_Owner->m_Parent != nullptr)
		return (m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * m_Rotation.WorldZ().ToDir4()).Float3Part();
	return m_Rotation.WorldZ();
}

float3 CTransform::GetUp() const
{
	if (m_Owner->m_Parent != nullptr)
		return (m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * m_Rotation.WorldY().ToDir4()).Float3Part();
	return m_Rotation.WorldY();
}

float3 CTransform::GetRight() const
{
	if (m_Owner->m_Parent != nullptr)
		return (m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * m_Rotation.WorldX().ToDir4()).Float3Part();
	return m_Rotation.WorldX();
}