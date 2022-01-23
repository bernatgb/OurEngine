#include "Quadtree.h"

/* QUADTREENODE */

QuadtreeNode::QuadtreeNode(AABB _aabb, QuadtreeNode* _parent)
{
	m_nodeAABB = _aabb;
	m_parent = _parent;
	m_children[0] = m_children[1] = m_children[2] = m_children[3] = nullptr;
}

QuadtreeNode::~QuadtreeNode()
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_children[i] != nullptr)
			delete m_children[i];
	}
}

void QuadtreeNode::InsertGO(GameObject* go)
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

void QuadtreeNode::EraseGO(GameObject* go)
{
	std::list<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), go);
	if (it != gameObjects.end())
	{
		gameObjects.erase(it);
	}
	if (!IsLeaf())
	{
		for (int i = 0; i < 4; ++i)
			m_children[i]->EraseGO(go);
	}
}

void QuadtreeNode::RedistributeChildren()
{
	for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end();)
	{
		GameObject* go = *it;

		bool intersects[4];
		for (int i = 0; i < 4; ++i)
		{
			intersects[i] = m_children[i]->m_nodeAABB.Intersects(go->m_aabb);
		}

		if (intersects[0] && intersects[1] && intersects[2] && intersects[3])
			++it;
		else
		{
			it = gameObjects.erase(it);
			for (int i = 0; i < 4; ++i)
			{
				if (intersects[i])
					m_children[i]->InsertGO(go);
			}
		}
	}
}

void QuadtreeNode::CreateChildren()
{
	/// <AABB>
	/// MAX
	/// NW	NE
	/// SW	SE
	///		MIN
	/// </AABB>
	
	vec minPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.MinY(), m_nodeAABB.CenterPoint().z);
	vec maxPoint = m_nodeAABB.maxPoint;
	AABB aabb = AABB(minPoint, maxPoint);
	m_children[0] = new QuadtreeNode(aabb, this);
	
	minPoint = vec(m_nodeAABB.MinX(), m_nodeAABB.MinY(), m_nodeAABB.CenterPoint().z);
	maxPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.MaxY(), m_nodeAABB.MaxZ());
	aabb = AABB(minPoint, maxPoint);
	m_children[1] = new QuadtreeNode(aabb, this);

	minPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.MinY(), m_nodeAABB.MinZ());
	maxPoint = vec(m_nodeAABB.MaxX(), m_nodeAABB.MaxY(), m_nodeAABB.CenterPoint().z);
	aabb = AABB(minPoint, maxPoint);
	m_children[2] = new QuadtreeNode(aabb, this);

	minPoint = m_nodeAABB.minPoint;
	maxPoint = vec(m_nodeAABB.CenterPoint().x, m_nodeAABB.MaxY(), m_nodeAABB.CenterPoint().z);
	aabb = AABB(minPoint, maxPoint);
	m_children[3] = new QuadtreeNode(aabb, this);
}

bool QuadtreeNode::IsFull()
{
	return gameObjects.size() >= MAX_GAME_OBJECTS;
}

bool QuadtreeNode::IsMin()
{
	return m_nodeAABB.HalfSize().LengthSq() <= MIN_QUADTREENODE_AREA;
}

bool QuadtreeNode::IsLeaf()
{
	return m_children[0] == nullptr;
}

void QuadtreeNode::SetObejctsInFrustum(Plane planes[6])
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
				//goToPaint.push_back(go);
				go->m_InFrustum = true;
			}
		}

		for (int i = 0; i < 4; ++i)
		{
			if (m_children[i] != nullptr)
				m_children[i]->SetObejctsInFrustum(planes); //m_children[i]->GetObjectsToPaint(planes, goToPaint);
		}
	}
}

bool QuadtreeNode::Intersects(Plane planes[6], float3 cornerPoints[8])
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

void QuadtreeNode::Clear()
{
	if (IsLeaf())
		delete this;
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			m_children[i]->Clear();
		}
	}

	delete this;
}

/* QUADTREE */

Quadtree::Quadtree()
{
	m_root = nullptr;
}

Quadtree::~Quadtree()
{
	delete m_root;
}

void Quadtree::InsertGO(GameObject* go)
{
	// Check inside root
	m_root->InsertGO(go);

	/*

	if (!m_root->IsFull())
	{
		m_root->AddChild(go);
	}
	else 
	{
		QuadtreeNode* qtn = m_root->GetBetterLocationForNewNode(go);
		qtn->AddChild(go);
	}
	*/
}

void Quadtree::EraseGO(GameObject* go)
{
	m_root->EraseGO(go);

	//QuadtreeNode* qtn = m_root->FindNode(go);
	//qtn->GetParent()->DeleteChild(go);
}

void Quadtree::SetBoundaries(AABB aabb)
{
	m_root = new QuadtreeNode(aabb, nullptr);
}

void Quadtree::SetObejctsInFrustum(Frustum* frustum)
{
	Plane planes[6];
	frustum->GetPlanes(planes);

	//std::list<GameObject*> goToPaint;

	//m_root->SetObejctsInFrustum(planes, goToPaint);
	m_root->SetObejctsInFrustum(planes);
}

void Quadtree::Clear()
{
	m_root->Clear();
}
