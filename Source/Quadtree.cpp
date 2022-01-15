#include "Quadtree.h"

/* QUADTREENODE */

QuadtreeNode::QuadtreeNode(GameObject* go)
{
	if (go != nullptr)
	{
		m_nodeName = go->m_Name;
		m_nodeAABB = go->m_aabb;
		for (int i = 0; i < 4; ++i)
		{
			m_children[i] = nullptr;
		}
	}
}

QuadtreeNode::~QuadtreeNode()
{
	delete m_nodeName;
	delete m_parent;
	for (int i = 0; i < 4; ++i)
	{
		if (m_children[i] != nullptr)
			delete m_children[i];
	}
}

void QuadtreeNode::AddChild(GameObject* go)
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_children[i] != nullptr)
			m_children[i] = new QuadtreeNode(go);
	}

	RedistributeChildren();
}

void QuadtreeNode::DeleteChild(GameObject* go) // TODO: Revise
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_children[i]->m_nodeGUID == go->m_GUID)
			m_children[i] = nullptr;
	}

	RedistributeChildren();
}

void QuadtreeNode::RedistributeChildren()
{
	int numChildren = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (m_children[i] != nullptr)
			++numChildren;
	}
		
	std::vector <std::pair<float, QuadtreeNode*> > zCenter(numChildren);
	for (int i = 0; i < numChildren; ++i)
		zCenter[i] = std::pair<float, QuadtreeNode*>(m_children[i]->m_nodeAABB.CenterPoint().z, m_children[i]);

	std::sort(zCenter.begin(), zCenter.end()); // Ordered from North to South
			
	for (int i = 0; i < numChildren; ++i)
	{
		m_children[i] = zCenter[i].second;

		if (i % 2 == 1 && m_children[i - 1]->m_nodeAABB.CenterPoint().x < m_children[i]->m_nodeAABB.CenterPoint().x)
				std::swap(m_children[i - 1], m_children[i]); // Ordered from West to East
	}
}

QuadtreeNode* QuadtreeNode::GetParent()
{
	return m_parent;
}

QuadtreeNode* QuadtreeNode::GetBetterLocationForNewNode(GameObject* go)
{
	float3 centerPoint = go->m_aabb.CenterPoint();
	
	if (IsFull())
	{
		std::vector <std::pair<int, int> > dist(4);
		for (int i = 0; i < 4; ++i)
			dist[i] = std::pair<int, int>(m_children[i]->m_nodeAABB.Distance(centerPoint), i);

		std::sort(dist.begin(), dist.end());
		
		return m_children[dist[0].second]->GetBetterLocationForNewNode(go);
	}
	else
		return this;
}

QuadtreeNode* QuadtreeNode::FindNode(GameObject* go)
{
	for (int i = 0; i < 4; ++i)
	{
		if (go->m_GUID == m_children[i]->m_nodeGUID)
			return m_children[i];
		else
		{
			// Assuming this branch is full because otherwise we would have already found it.
			float3 centerPoint = go->m_aabb.CenterPoint();
			std::vector <std::pair<int, int> > dist(4);
			for (int i = 0; i < 4; ++i)
				dist[i] = std::pair<int, int>(m_children[i]->m_nodeAABB.Distance(centerPoint), i);

			std::sort(dist.begin(), dist.end());

			return m_children[dist[0].second]->FindNode(go);
		}
	}
}

bool QuadtreeNode::IsFull()
{
	int numChildren = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (m_children[i] != nullptr)
			++numChildren;
	}

	return numChildren == 4;
}

/* QUADTREE */

Quadtree::Quadtree(GameObject* root)
{
	m_root = new QuadtreeNode(root);
}

Quadtree::~Quadtree()
{
	delete m_root;
}

void Quadtree::AddNode(GameObject* go)
{
	if (!m_root->IsFull())
	{
		m_root->AddChild(go);
	}
	else 
	{
		QuadtreeNode* qtn = m_root->GetBetterLocationForNewNode(go);
		qtn->AddChild(go);
	}
}

void Quadtree::DeleteNode(GameObject* go)
{
	QuadtreeNode* qtn = m_root->FindNode(go);
	qtn->GetParent()->DeleteChild(go);
}