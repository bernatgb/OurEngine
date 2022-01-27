#pragma once
#include "GameObject.h"

#include <list>

#define MAX_GAME_OBJECTS 8
#define MIN_OCTOTREENODE_VOLUME 50.0f;

class OctotreeNode
{
public:
	OctotreeNode(AABB _aabb, OctotreeNode* _parent);
	~OctotreeNode();

	void InsertGO(GameObject* go);
	void EraseGO(GameObject* go);
	void RedistributeChildren();

	void CreateChildren();

	bool IsFull();
	bool IsMin();
	bool IsLeaf();

	void SetObejctsInFrustum(Plane planes[6]);
	bool Intersects(Plane planes[6], float3 cornerPoints[8]);

	AABB GetAABB() { return m_nodeAABB; };
	const OctotreeNode* GetParent() const { return m_parent; };
	OctotreeNode** GetChildren() { return m_children; };
	std::list<GameObject*> GetGameObjectsInThisNode() const { return gameObjects; };

private:
	AABB m_nodeAABB;
	OctotreeNode* m_parent = nullptr;
	OctotreeNode* m_children[8]; // 0 = NWU, 1 = NEU, 2 = SWU, 3 = SEU, 4 = NWD, 5 = NED, 6 = SWD, 7 = SED.
	std::list<GameObject*> gameObjects;
};

class Octotree
{
public:
	Octotree();
	~Octotree();

	void InsertGO(GameObject* go);
	void EraseGO(GameObject* go);

	void SetBoundaries(AABB aabb);

	void SetObejctsInFrustum(Frustum* frustum);

	void Clear();

	OctotreeNode* GetRoot() const { return m_root; };

private:
	OctotreeNode* m_root = nullptr;
};
