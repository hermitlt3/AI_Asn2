#pragma once
#include "SingletonTemplate.h"
#include "GameObject.h"
#include <list>

using std::list;
class GameObjectManager : public Singleton<GameObjectManager> {
	friend Singleton<GameObjectManager>;

public:
	list<GameObject*> m_goList;
};

/*********** Physics *************/
static bool CheckCollision(GameObject *go, GameObject *other, double dt);
//float CheckCollision2(GameObject* go, GameObject* other);
static void CollisionResponse(GameObject *go, GameObject *other, double dt);
/*********************************/

bool CheckCollision(GameObject *go, GameObject *other, double dt)
{
	switch (other->type)
	{
	case GameObject::GO_PLAYER:
	{
								  float lengthSquared = ((go->pos + go->vel * (float)dt) - (other->pos - other->vel *(float)dt)).LengthSquared();
								  float combinedRadSq = (go->scale.x + other->scale.x) * (go->scale.x + other->scale.x);
								  Vector3 relativeVelocity = go->vel - other->vel;
								  Vector3 relativeDisplacement = other->pos - go->pos;
								  return ((lengthSquared < combinedRadSq) && (relativeVelocity.Dot(relativeDisplacement) > 0));
								  break;
	}
	}
	return false;
}

void CollisionResponse(GameObject *go, GameObject *other, double dt)
{
	switch (other->type)
	{
	case GameObject::GO_PLAYER:
	{
								  //Exercise 8b: store values in auditing variables
								  float m1 = go->mass;
								  float m2 = other->mass;
								  Vector3 u1 = go->vel;
								  Vector3 u2 = other->vel;

								  Vector3 u1N, u2N, N;
								  N = (other->pos - go->pos).Normalized();

								  u1N = u1.Dot(N) * N;
								  u2N = u2.Dot(N) * N;

								  go->vel = u1 + (2 * m2) / (m1 + m2) * (u2N - u1N);
								  other->vel = u2 + (2 * m1) / (m1 + m2) * (u1N - u2N);
								  break;
	}
	}
}
