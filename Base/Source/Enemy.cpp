#include "Enemy.h"
#include "GameObjectManager.h"
#include <vector>

Enemy::Enemy() :
isLeader(false),
isSent(false),
speed(3.f)
{
	health = 50;
}

Enemy::~Enemy()
{

}

void Enemy::Update(double dt)
{
	Vector3 getNearestFriendly(FLT_MAX, FLT_MAX, FLT_MAX);
	for (std::vector<GameObject *>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->type == GameObject::GO_PRIEST || go->type == GameObject::GO_GUARDIAN) {
			if ((pos - go->pos).Length() < getNearestFriendly.Length()) {
				getNearestFriendly = pos - go->pos;
			}
		}
	}
		vel = -getNearestFriendly.Normalized() * speed;
	
	if (health < 999 && isLeader)
	{
		if (!isSent) {
			SendMessage("AMBUSH");
			isSent = true;
		}
	}
	if (health <= 0)
		active = false;
}

void Enemy::OnNotification(const std::string& str)
{
	if (str == "AMBUSH" && !isLeader)
	{
		if (active == false)
			active = true;	
	}
}

