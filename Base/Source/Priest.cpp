#include "Priest.h"
#include "GameObjectManager.h"
#include <vector>

Priest::Priest() :
currState(IDLE),
innerProximity(5.f),
outerProximity(10.f),
nextHealth(health),
timer(0.0)
{
	health = 100;
}

Priest::~Priest()
{

}

void Priest::OnNotification(const std::string& msg)
{
	if (msg == "INJURED")
	{
		currState = HEAL;
	}
	else if (msg == "UNINJURED")
	{
		currState = IDLE;
	}
}

void Priest::Update(double dt)
{
	if (currState == HEAL)
	{
		vel = Vector3(-5, 0, 0);
	}
	if (currState == IDLE)
	{
		vel = Vector3(5, 0, 0);
	}
	if (currState == RUN)
	{
		vel = IsEnemiesInInnerP();
	}
	if (currState == DIE)
	{
		vel = Vector3(0, -5, 0);
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
			SendMessage("ATTACKED");
		}
		if (currState == RUN) {
			if (!IsEnemiesInInnerP())
			{
				currState = IDLE;
			}
		}
	}
}

Vector3 Priest::IsEnemiesInOuterP()
{
	Vector3 distance = 0.f;
	for (std::vector<GameObject *>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->type == GameObject::GO_PRIEST)
			continue;
		if ((go->pos - pos).Length() <= outerProximity)
		{
			 distance += go->pos - pos;
		}
	}
	return distance.Normalized();
}

bool Priest::IsEnemiesInInnerP()
{
	for (std::vector<GameObject *>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->type == GameObject::GO_PRIEST)
			continue;
		if ((go->pos - pos).Length() <= innerProximity)
		{
			return true;
		}
	}
	return false;
}