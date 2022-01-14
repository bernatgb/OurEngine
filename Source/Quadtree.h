#pragma once
#include "GameObject.h"

class QuadtreeNode
{
public:
	QuadtreeNode(GameObject* go);
	~QuadtreeNode();

	void AddChild(GameObject* go);
	void DeleteChild(GameObject* go);
	void RedistributeChildren();
	
	QuadtreeNode* GetParent();

	QuadtreeNode* GetBetterLocationForNewNode(GameObject* go);
	QuadtreeNode* FindNode(GameObject* go);

	bool m_full = false;

private:
	char* m_nodeName;
	AABB m_nodeAABB;
	QuadtreeNode* m_parent;
	QuadtreeNode* m_children[4]; // 0 = NW, 1 = NE, 2 = SW, 3 = SE.
};

class Quadtree
{
public:
	Quadtree();
	~Quadtree();

	void AddNode(GameObject* go);
	void DeleteNode(GameObject* go);

private:
	QuadtreeNode* m_root = nullptr;
};