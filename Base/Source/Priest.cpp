#include "Priest.h"
#include "GameObjectManager.h"
#include "Guardian.h"
#include <vector>

Priest::Priest() :
currState(IDLE),
innerProximity(15.f),
outerProximity(30.f),
nextHealth(health),
timer(0.0),
speed(5.f)
{
	health = 100;
	maxhealth = health;
}

Priest::~Priest()
{

}

void Priest::OnNotification(const std::string& msg)
{
	if (msg == "INJURED")
	{
		timer = 0.0;
		currState = HEAL;
	}
	else if (msg == "UNINJURED")
	{
		currState = IDLE;
	}
}

void Priest::Update(double dt)
{
	switch (currState) {
	case HEAL:
	{
				 HealsGuardian(dt);
				 break;
	}
	case IDLE:
	{
				 ReturnToIdle(dt);
				 break;
	}
	case RUN:
	{
				vel = IsEnemiesInOuterP() * speed;
				break;
	}
	case DIE:
	{
				scale.x -= dt; scale.y -= dt; scale.z -= dt;
				if (scale.x <= Math::EPSILON) {
					active = false;
				}
	}
	}
	// Check if the health in this frame is not the same as the one in the next frame
	nextHealth = health;
}

void Priest::FSM()
{
	// Unless you "resurrect", or you die
	if (health <= 0)
		currState = DIE;

	// If health decreases due to enemy
	else
	{
		if (health != nextHealth) {
			currState = RUN;
			SendMessage("UNDER ATTACK");
		}
		switch (currState){
		case RUN:
		{
				  if (!IsEnemiesInInnerP())
				  {
					  currState = IDLE;
					  SendMessage("UNHARMED");
				  }
				  break;
		}
		case HEAL:
		{

					 break;
		}
		case IDLE:
		{
					 // In OnNotification();
					 break;
		}
		}
	}
}

Vector3 Priest::IsEnemiesInOuterP()
{
	Vector3 distance = 0.f;
	for (std::vector<GameObject *>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it) {
		GameObject *go = (GameObject *)*it;
		if (go->type == GameObject::GO_PRIEST || go->type == GameObject::GO_GUARDIAN)
			continue;
		if ((go->pos + go->scale - pos - scale).Length() <= outerProximity) {
			 distance += go->pos - pos;
		}
	}
	return -distance.Normalized();
}

bool Priest::IsEnemiesInInnerP()
{
	for (std::vector<GameObject *>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it) {
		GameObject *go = (GameObject *)*it;
		if (go->type == GameObject::GO_PRIEST || go->type == GameObject::GO_GUARDIAN)
			continue;
		if ((go->pos + go->scale - pos - scale).Length() <= innerProximity) {
			return true;
		}
	}
	return false;
}

void Priest::HealsGuardian(double dt)
{
	if (guardian->health < guardian->maxhealth) {
		if ((guardian->pos - pos).Length() <= guardian->scale.x + scale.x) {
			timer += dt;
			vel.SetZero();
		}
		else
			vel = (guardian->pos - pos).Normalized() * speed;

		if (timer > 1.0) {
			guardian->health += 5;
			timer = 0.0;
		}
	}
	else {
		guardian->health = guardian->maxhealth;
		SendMessage("UNINJURED");
	}
}

void Priest::ReturnToIdle(double dt)
{
	if ((guardian->pos - pos).Length() >= innerProximity)
		vel.SetZero();
	else
		vel = (pos - guardian->pos).Normalized() * speed;
}
