#include "Quadtree.h"

/* QUADTREENODE */

QuadtreeNode::QuadtreeNode(GameObject* go)
{
	m_nodeName = go->m_Name;
	m_nodeAABB = go->m_aabb;
	for (int i = 0; i < 4; ++i)
	{
		m_children[i] = nullptr;
	}
}

QuadtreeNode::~QuadtreeNode()
{
	delete m_nodeName;
	delete m_parent;
	delete m_children[4];
}

void QuadtreeNode::AddChild(GameObject* go)
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_children[i] != nullptr)
			m_children[i] = &QuadtreeNode(go);
	}

	RedistributeChildren();
}

void QuadtreeNode::DeleteChild(GameObject* go) // TODO: Revise
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_children[i] == &QuadtreeNode(go))
			m_children[i] = nullptr;
	}

	RedistributeChildren();
}

void QuadtreeNode::RedistributeChildren()
{
	float auxMinX, auxMinZ, auxMaxX, auxMaxZ;
	bool childFound = false;
	QuadtreeNode* auxChildren[4];

	for (int i = 0; i < 4; ++i)
	{
		if (m_children[i] != nullptr)
		{
			if (!childFound)
			{
				auxMinX = m_children[i]->m_nodeAABB.minPoint.x;
				auxMinZ = m_children[i]->m_nodeAABB.minPoint.z;
				auxMaxX = m_children[i]->m_nodeAABB.maxPoint.x;
				auxMaxZ = m_children[i]->m_nodeAABB.maxPoint.z;

				auxChildren[0] = m_children[i];
			}
			else
			{
				for (int j = 0; j < i; ++j)
				{
					if (m_children[i]->m_nodeAABB.minPoint.x < auxChildren[j]->m_nodeAABB.minPoint.x); // Western than aux
					if (m_children[i]->m_nodeAABB.minPoint.x > auxChildren[j]->m_nodeAABB.minPoint.x); // Eastern than aux
					if (m_children[i]->m_nodeAABB.minPoint.z < auxChildren[j]->m_nodeAABB.minPoint.z); // Nothern than aux
					if (m_children[i]->m_nodeAABB.minPoint.x < auxChildren[j]->m_nodeAABB.minPoint.x); // Southern than aux
				}
			}
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		m_children[i] = auxChildren[i];
	};
}

QuadtreeNode* QuadtreeNode::GetParent()
{
	return m_parent;
}

QuadtreeNode* QuadtreeNode::GetBetterLocationForNewNode(GameObject* go)
{
	QuadtreeNode* qtn = nullptr;
	//Find it 
	return qtn;
}

QuadtreeNode* QuadtreeNode::FindNode(GameObject* go)
{
	QuadtreeNode* qtn = nullptr;
	//Find it 
	return qtn;
}

/* QUADTREE */

Quadtree::Quadtree()
{
	GameObject* gg;
	m_root = &QuadtreeNode(gg);
}

Quadtree::~Quadtree()
{
	delete m_root;
}

void Quadtree::AddNode(GameObject* go)
{
	if (!m_root->m_full)
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