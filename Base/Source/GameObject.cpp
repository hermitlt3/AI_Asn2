
#include "GameObject.h"
#include "Mtx44.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
: type(typeValue),
pos(0, 0, 0),
vel(0, 0, 0),
normal(0, 0, 0),
scale(1, 1, 1),
active(false),
mass(1.f)
{
}

GameObject::~GameObject()
{
}

void GameObject::SetGO(

	GameObject::GAMEOBJECT_TYPE type,
	Vector3 scale,
	Vector3 rotate,
	Vector3 pos)
{
	Mtx44 ro;
	ro.SetToRotation(rotate.y, 0, 1, 0);
	ro.SetToRotation(rotate.x, 1, 0, 0);
	ro.SetToRotation(rotate.z, 0, 0, 1);

	this->scale = scale;
	this->normal = ro * Vector3(1, 0, 0);
	this->type = type;
	this->pos = pos;
	this->active = true;
}
