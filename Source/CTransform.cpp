#include "CTransform.h"

#include "GameObject.h"

#include "SceneImporter.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// TODO: show all decimals in imguis

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

void Float3ToRange0_360(float3& _values) 
{
	_values.x = fmod((_values.x < 0.0f) ? _values.x + 360 : _values.x, 360);
	_values.y = fmod((_values.y < 0.0f) ? _values.y + 360 : _values.y, 360);
	_values.z = fmod((_values.z < 0.0f) ? _values.z + 360 : _values.z, 360);
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
	// Change AccumulativeModelMatrix
	if (m_Owner->m_Parent != nullptr)
		m_AccumulativeModelMatrix = m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix * m_ModelMatrix;
	else
		m_AccumulativeModelMatrix = m_ModelMatrix;
}

void CTransform::DrawImGui(int index)
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Position", &m_Position[0], 0.5f))
		{
			RecalculateModelMatrix();
		}
		if (ImGui::DragFloat3("Rotation", &m_RotationEuler[0], 5.0f))
		{
			Float3ToRange0_360(m_RotationEuler);

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
	// Recalculate the new modelMatrix
	m_ModelMatrix = m_Owner->m_Parent->m_Transform->m_AccumulativeModelMatrix.Inverted() * _newAccumulativeModelMatrix;

	// Set all the values
	m_Position = m_ModelMatrix.Col3(3);
	m_RotationEuler = m_ModelMatrix.ToEulerXYZ();
	m_Rotation = Quat::FromEulerXYZ(m_RotationEuler.x, m_RotationEuler.y, m_RotationEuler.z);
	m_RotationEuler.x = RadToDeg(m_RotationEuler.x);
	m_RotationEuler.y = RadToDeg(m_RotationEuler.y);
	m_RotationEuler.z = RadToDeg(m_RotationEuler.z);
	Float3ToRange0_360(m_RotationEuler);
	m_Scale = m_ModelMatrix.GetScale();

	// Notify movement
	NotifyMovement();

	// Notify movement to parent
	m_Owner->NotifyMovement(true);
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

	// Notify movement
	NotifyMovement();
	// Notify movement to parent
	m_Owner->NotifyMovement(true);
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