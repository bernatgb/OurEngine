#include "Octotree.h"

/* OCTOTREENODE */

OctotreeNode::OctotreeNode(AABB _aabb, OctotreeNode* _parent)
{
	m_nodeAABB = _aabb;
	m_parent = _parent;
	m_children[0] = m_children[1] = m_children[2] = m_children[3] = m_children[4] = m_children[5] = m_children[6] = m_children[7] = nullptr;
}

OctotreeNode::~OctotreeNode()
{
	for (int i = 0; i < 8; ++i)
	{
		if (m_children[i] != nullptr)
			delete m_children[i];
	}
}

void OctotreeNode::InsertGO(GameObject* go)
{
	if (IsLeaf() && !IsFull() || IsMin())
	{
		gameObjects.push_back(go);
	}
	else
	{
		if (IsLeaf())
			CreateChildren();

		gameObjects.push_back(go);
		RedistributeChildren();
	}
}

void OctotreeNode::EraseGO(GameObject* go)
{
	std::list<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), go);
	if (it != gameObjects.end())
	{
		gameObjects.erase(it);
	}
	if (!IsLeaf())
	{
		for (int i = 0; i < 8; ++i)
			m_children[i]->EraseGO(go);
	}
}

void OctotreeNode::RedistributeChildren()
{
	for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end();)
	{
		GameObject* go = *it;

		bool intersects[8];
		for (int i = 0; i < 8; ++i)
		{
			intersects[i] = m_children[i]->m_nodeAABB.Intersects(go->m_aabb);
		}

		if (intersects[0] && intersects[1] && intersects[2] && intersects[3] && intersects[4] && intersects[5] && intersects[6] && intersects[7])
			++it;
		else
		{
			it = gameObjects.erase(it);
			for (int i = 0; i < 8; ++i)
			{
				if (intersects[i])
					m_children[i]->InsertGO(go);
			}
		}
	}
}

void OctotreeNode::CreateChildren()
{
	/// <AABB>		<AABB>
	/// MAX			MAX
	/// NWU	NEU		NWD	NED
	/// SWU	SEU		SWD	SED
	///		MIN			MIN
	/// </AABB>		</AABB>

	vec minPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.CenterPoint().y, m_nodeAABB.CenterPoint().z);
	vec maxPoint = m_nodeAABB.maxPoint;
	AABB aabb = AABB(minPoint, maxPoint);
	m_children[0] = new OctotreeNode(aabb, this);

	minPoint = vec(m_nodeAABB.MinX(), m_nodeAABB.CenterPoint().y, m_nodeAABB.CenterPoint().z);
	maxPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.MaxY(), m_nodeAABB.MaxZ());
	aabb = AABB(minPoint, maxPoint);
	m_children[1] = new OctotreeNode(aabb, this);

	minPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.CenterPoint().y, m_nodeAABB.MinZ());
	maxPoint = vec(m_nodeAABB.MaxX(), m_nodeAABB.MaxY(), m_nodeAABB.CenterPoint().z);
	aabb = AABB(minPoint, maxPoint);
	m_children[2] = new OctotreeNode(aabb, this);

	minPoint = vec(m_nodeAABB.MinX(), m_nodeAABB.CenterPoint().y, m_nodeAABB.MinZ());
	maxPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.MaxY(), m_nodeAABB.CenterPoint().z);
	aabb = AABB(minPoint, maxPoint);
	m_children[3] = new OctotreeNode(aabb, this);
	
	minPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.MinY(), m_nodeAABB.CenterPoint().z);
	maxPoint = vec(m_nodeAABB.MaxX(), m_nodeAABB.CenterPoint().y, m_nodeAABB.MaxZ());
	aabb = AABB(minPoint, maxPoint);
	m_children[4] = new OctotreeNode(aabb, this);

	minPoint = vec(m_nodeAABB.MinX(), m_nodeAABB.MinY(), m_nodeAABB.CenterPoint().z);
	maxPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.CenterPoint().y, m_nodeAABB.MaxZ());
	aabb = AABB(minPoint, maxPoint);
	m_children[5] = new OctotreeNode(aabb, this);

	minPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.MinY(), m_nodeAABB.MinZ());
	maxPoint = vec(m_nodeAABB.MaxX(), m_nodeAABB.CenterPoint().y, m_nodeAABB.CenterPoint().z);
	aabb = AABB(minPoint, maxPoint);
	m_children[6] = new OctotreeNode(aabb, this);

	minPoint = m_nodeAABB.minPoint;
	maxPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.CenterPoint().y, m_nodeAABB.CenterPoint().z);
	aabb = AABB(minPoint, maxPoint);
	m_children[7] = new OctotreeNode(aabb, this);
}

bool OctotreeNode::IsFull()
{
	return gameObjects.size() >= MAX_GAME_OBJECTS;
}

bool OctotreeNode::IsMin()
{
	return m_nodeAABB.HalfSize().LengthSq() <= MIN_OCTOTREENODE_VOLUME; // Revise
}

bool OctotreeNode::IsLeaf()
{
	return m_children[0] == nullptr;
}

void OctotreeNode::SetObejctsInFrustum(Plane planes[6])
{
	float3 cornerPoints[8];
	m_nodeAABB.GetCornerPoints(cornerPoints);

	if (Intersects(planes, cornerPoints))
	{

		for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
		{
			GameObject* go = *it;

			float3 goCornerPoints[8];
			go->m_aabb.GetCornerPoints(goCornerPoints);

			if (!go->m_InFrustum && Intersects(planes, goCornerPoints))
			{
				go->m_InFrustum = true;
			}
		}

		for (int i = 0; i < 8; ++i)
		{
			if (m_children[i] != nullptr)
				m_children[i]->SetObejctsInFrustum(planes);
		}
	}
}

bool OctotreeNode::Intersects(Plane planes[6], float3 cornerPoints[8])
{
	int out;
	for (int i = 0; i < 6; ++i) // For all planes
	{
		out = 0;
		for (int j = 0; j < 8; ++j) // For all points
			out += planes[i].IsOnPositiveSide(cornerPoints[j]);

		if (out == 8)
			return false;
	}
	return true;
}

/* OCTOTREE */

Octotree::Octotree()
{
	m_root = nullptr;
}

Octotree::~Octotree()
{
	delete m_root;
}

void Octotree::InsertGO(GameObject* go)
{
	// Check inside root
	m_root->InsertGO(go);
}

void Octotree::EraseGO(GameObject* go)
{
	m_root->EraseGO(go);
}

void Octotree::SetBoundaries(AABB aabb)
{
	m_root = new OctotreeNode(aabb, nullptr);
}

void Octotree::SetObejctsInFrustum(Frustum* frustum)
{
	Plane planes[6];
	frustum->GetPlanes(planes);

	m_root->SetObejctsInFrustum(planes);
}

void Octotree::Clear()
{
	AABB boundaries = m_root->GetAABB();
	delete m_root;
	SetBoundaries(boundaries);
}
