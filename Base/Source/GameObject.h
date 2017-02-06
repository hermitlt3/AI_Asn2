#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"
#include "Vertex.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_PLAYER,
		GO_PRIEST,
		GO_GUARDIAN,
		GO_ENEMY,

		GO_TOTAL, //must be last
	};

	GAMEOBJECT_TYPE type;

	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	Vector3 normal;

	bool active;

	float mass;
	int health;
	int maxhealth;
	int mana;
	int maxmana;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_NONE);
	virtual ~GameObject();

	void SetGO(GameObject::GAMEOBJECT_TYPE type = GameObject::GO_NONE,
		Vector3 scale = (1.f, 1.f, 1.f),
		Vector3 rotate = (0.f, 0.f, 0.f),
		Vector3 pos = (0.f, 0.f, 0.f),
		bool _active = true
				);
};

#endif