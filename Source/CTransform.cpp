#include "CTransform.h"

#include "GameObject.h"

#include "SceneImporter.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Quadtree.h"

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

Component* CTransform::GetAClone(GameObject* _owner)
{
	CTransform* newCCTransform = new CTransform(m_Enabled, _owner);

	newCCTransform->m_AccumulativeModelMatrix = m_AccumulativeModelMatrix;

	newCCTransform->m_ModelMatrix = m_ModelMatrix;
	newCCTransform->m_Position = m_Position;
	newCCTransform->m_RotationEuler = m_RotationEuler;
	newCCTransform->m_Scale = m_Scale;
	newCCTransform->m_Rotation = m_Rotation;

	return newCCTransform;
}

void CTransform::NotifyMovement()
{
	Quadtree* qt = App->scene->GetQuadtree();
	GameObject* go = m_Owner;
	qt->EraseGO(go);

	//Change AccumulativeModelMatrix
	if (m_Owner->m_Parent != nullptr)
		m_AccumulativeModelMatrix = m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * m_ModelMatrix;
	else
		m_AccumulativeModelMatrix = m_ModelMatrix;

	//TODO: NOTIFY GO OWNER

	//Notify other components
	for (unsigned int i = 0; i < m_Owner->m_Components.size(); ++i)
	{
		m_Owner->m_Components[i]->NotifyMovement();
		if (go->m_Components[i]->m_Type == ComponentType::MESH)
			qt->InsertGO(go);
	}

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

void CTransform::GizmoTransformChange(float4x4 _newAccumulativeModelMatrix)
{
	// TODO: calculate the new modelMatrix and call NotifyMovement();
	//m_ModelMatrix = _newAccumulativeModelMatrix * m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix.Inverted();

	//m_ModelMatrix = m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * _newAccumulativeModelMatrix.Inverted();
	m_ModelMatrix = m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix.Inverted() * _newAccumulativeModelMatrix;

	//todo: get position rotation and scale
	m_Position = m_ModelMatrix.Col3(3);
	m_RotationEuler = m_ModelMatrix.ToEulerXYZ();
	m_Rotation = Quat::FromEulerXYZ(m_RotationEuler.x, m_RotationEuler.y, m_RotationEuler.z);
	m_RotationEuler.x = RadToDeg(m_RotationEuler.x);
	m_RotationEuler.y = RadToDeg(m_RotationEuler.y);
	m_RotationEuler.z = RadToDeg(m_RotationEuler.z);
	m_Scale = m_ModelMatrix.GetScale();

	NotifyMovement();

	/*m_AccumulativeModelMatrix = _newAccumulativeModelMatrix;

	for (unsigned int i = 0; i < m_Owner->m_Components.size(); ++i)
		m_Owner->m_Components[i]->NotifyMovement();

	//Notify owner children
	for (unsigned int i = 0; i < m_Owner->m_Children.size(); ++i)
		m_Owner->m_Children[i]->m_Transform->NotifyMovement();*/
}

void CTransform::Copy(const CTransform* _transform)
{
	m_AccumulativeModelMatrix = _transform->m_AccumulativeModelMatrix;

	m_ModelMatrix = _transform->m_ModelMatrix;
	m_Position = _transform->m_Position;
	m_RotationEuler = _transform->m_RotationEuler;
	m_Scale = _transform->m_Scale;
	m_Rotation = _transform->m_Rotation;
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

float3 CTransform::GetForward(bool normalize) const
{
	float3 forward;
	if (m_Owner->m_Parent != nullptr)
		forward = (m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * m_Rotation.WorldZ().ToDir4()).Float3Part();
	else
		forward = m_Rotation.WorldZ();

	if (normalize) return forward.Normalized();
	return forward;
}

float3 CTransform::GetUp(bool normalize) const
{
	float3 up;
	if (m_Owner->m_Parent != nullptr)
		up = (m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * m_Rotation.WorldY().ToDir4()).Float3Part();
	else
		up = m_Rotation.WorldY();

	if (normalize) return up.Normalized();
	return up;
}

float3 CTransform::GetRight(bool normalize) const
{
	float3 right;
	if (m_Owner->m_Parent != nullptr)
		right = (m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * m_Rotation.WorldX().ToDir4()).Float3Part();
	else
		right = m_Rotation.WorldX();

	if (normalize) return right.Normalized();
	return right;
}