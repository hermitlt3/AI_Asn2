#include "Enemy.h"
#include "GameObjectManager.h"
#include <vector>

Enemy::Enemy() :
isLeader(false),
isSent(false),
speed(3.f),
currState(CHASE),
timer(0.0)
{
	health = 50;
}

Enemy::~Enemy()
{

}

void Enemy::Update(double dt)
{
	switch (currState)
	{
	case IDLE:
	{
				 break;
	}
	case CHASE:
	{
				  ChaseFriendly();
				  break;
	}
	case ATTACK:
	{
				   AttackTarget(dt);
				   break;
	}
	case DIE:
	{
				scale.x -= dt; scale.y -= dt; scale.z -= dt;
				if (scale.x <= Math::EPSILON) {
					active = false;
				}
				break;
	}

	}
	
	if (health <= 999 && isLeader)
	{
		if (!isSent) {
			SendMessage("AMBUSH");
			isSent = true;
		}
	}
	if (health <= 0)
	{
		for (vector<GameObject*>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end();) {
			if (*it == this) {
				it = GameObjectManager::GetInstance()->m_goList.erase(it);
				break;
			}
			else
				it++;
		}
	}
		
}

void Enemy::OnNotification(const std::string& str)
{
	if (str == "AMBUSH" && !isLeader)
	{
		if (active == false)
			active = true;	
	}
}

void Enemy::FSM()
{
	switch (currState)
	{
	case IDLE:
	{
				 break;
	}
	case CHASE:
	{
				  if (!target)
					  break;
				  if ((target->pos - pos).Length() <= target->scale.x + scale.x) {
					  currState = ATTACK;
				  }
				  break;
	}
	case ATTACK:
	{
				   if (!target)
					   break;
				   if ((target->pos - pos).Length() > target->scale.x + scale.x) {
					   currState = CHASE;
				   }
				   break;
	}
	case DIE:
	{
				break;
	}

	}
}

void Enemy::ChaseFriendly()
{
	Vector3 getNearestFriendly(FLT_MAX, FLT_MAX, FLT_MAX);
	for (std::vector<GameObject *>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->type == GameObject::GO_PRIEST || go->type == GameObject::GO_GUARDIAN) {
			if ((pos - go->pos).Length() < getNearestFriendly.Length()) {
				getNearestFriendly = pos - go->pos;
				target = (*it);
			}
		}
	}
	if (!target)
		return;
	vel = (target->pos - pos).Normalized() * speed;
}

void Enemy::AttackTarget(double dt)
{
	if (!target)
		return;
	vel.SetZero();
	timer += dt;
	if (timer > 2.0) {
		timer = 0.0;
		target->health -= 5;
	}
}