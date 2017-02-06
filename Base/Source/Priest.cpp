#include "Priest.h"
#include "GameObjectManager.h"
#include "Guardian.h"
#include <vector>

Priest::Priest() :
currState(IDLE),
innerProximity(25.f),
outerProximity(35.f),
nextHealth(health),
timer(0.0),
speed(5.f)
{
	health = 100;
	maxhealth = health;
	mana = 50;
	maxmana = mana;
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
		resetPos = pos;
	}
	else if (msg == "UNINJURED")
	{
		currState = RETURN;
	}
	else if (msg == "GUARDIAN DOWN")
	{
		currState = RESURRECT;
		//resetPos = pos;
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
	case RESURRECT:
	{
					  RevivesGuardian(dt);
					  break;
	}
	case REPLENISH:
	{
					  ReplenishMana(dt);
					  break;
	}
	case IDLE:
	{
				 vel.SetZero();
				 break;
	}
	case RUN:
	{
				vel = IsEnemiesInOuterP() * speed;
				break;
	}
	case DIE:
	{
				scale.x -= (float)dt * 3; scale.y -= (float)dt * 3; scale.z -= (float)dt * 3;
				if (scale.x <= Math::EPSILON) {
					active = false;
				}
				break;
	}
	case RETURN:
	{
				   ReturnToIdle(dt);
				   break;
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
		case IDLE:
		{
					 if ((float)mana / (float)maxmana < 0.5f) {
						 currState = REPLENISH;
						 resetPos = pos;
					 }
					 break;
		}
		case RUN:
		{
				  if (!IsEnemiesInInnerP())
				  {
					  currState = IDLE;
					  SendMessage("UNHARMED");
				  }
				  break;
		}
		case RESURRECT:
		{
						  if (guardian->active == true)
							  currState = RETURN;
						  break;
		}
		case REPLENISH:
		{
						  if (mana >= maxmana) {
							  mana = maxmana;
							  currState = RETURN;
						  }
						  break;
		}
		case RETURN:
		{
					   if ((guardian->pos - pos).Length() > innerProximity - 1.f && (guardian->pos - pos).Length() < innerProximity + 1.f)
						   currState = IDLE;

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
			guardian->health += 10;
			timer = 0.0;
			mana -= 5;
		}
	}
	else {
		guardian->health = guardian->maxhealth;
	}
}

void Priest::RevivesGuardian(double dt)
{
	if ((guardian->pos - pos).Length() <= guardian->scale.x + scale.x) {
		timer += dt;
		vel.SetZero();
	}
	else
		vel = (guardian->pos - pos).Normalized() * speed;

	if (timer > 3.0) {
		SendMessage("REVIVING");
		mana -= 30;
		timer = 0.0;
	}
}

void Priest::ReplenishMana(double dt)
{
	Vector3 offset;
	offset.Set(3, 3, 0);
	vel = (manaPos - pos).Normalized() * speed;
	if (pos.x > manaPos.x - offset.x && pos.x < manaPos.x + offset.x &&
		pos.y > manaPos.y - offset.y && pos.y < manaPos.y + offset.y) {
		vel.SetZero();

		timer += dt;
		if (timer > 0.2) {
			mana += 3;
			timer = 0.0;
		}
	}
}


void Priest::ReturnToIdle(double dt)
{
	if ((guardian->pos - pos).Length() < innerProximity + 1.f)
		vel = (pos - guardian->pos).Normalized() * speed;
	else if ((guardian->pos - pos).Length() > innerProximity - 1.f)
		vel = (guardian->pos - pos).Normalized() * speed;
	else
		vel.SetZero();
}
