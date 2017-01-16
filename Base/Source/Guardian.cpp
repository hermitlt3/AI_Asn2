#include "Guardian.h"
#include "Enemy.h"
#include "GameObjectManager.h"
#include <vector>
#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;

Guardian::Guardian() :
timer(0.0),
currState(IDLE), Speed(5.0f), Aggrorange(2.f)
{
	health = 100;
	maxhealth = health;
}

Guardian::~Guardian()
{

}

void Guardian::SetOriginalPosition(Vector3 position)
{
	OriginalPosition = position;
}

Vector3 Guardian::GetOriginalPosition()
{
	return OriginalPosition;
}

void Guardian::FSM()
{
	if (_target == nullptr)
	{
		return;
	}
	if (health <= 0)
	{
		currState = DIE;
	}
	else
	{
		switch (currState)
		{
		case CHASE:
		{
			break;
		}
		case ATTACK:
		{
			//This is in OnNotification();
			
			break;
		}
		case RETURN:
		{
			if (pos == OriginalPosition)
			{
				currState = IDLE;
				
			}

			break;
		}
		case IDLE:
		{
			//Here, The guard will check if he needs heals if he isn't at full health
			//State Transition for this state is in OnNotification();
			if (InAggroRange())
			{
				currState = ATTACK;
			}
			
			
		}
		}
	}
}

void Guardian::Update(double dt)
{
	
	if (_target == nullptr || _Priest == nullptr)
	{
		return;
	}
	switch (currState)
	{
	case IDLE:
	{
		break;
	}
	case CHASE:
	{
		GoToPriest(dt);
		break;
	}
	case RETURN:
	{
		Returnposition(dt);
		//cout << "Returning" << endl;
		break;
	}
	case ATTACK:
	{
		this->vel.SetZero();
		this->normal = (_target->pos - pos).Normalized();
		//cout << "Attacking" << endl;
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
}

void Guardian::OnNotification(const std::string& msg)
{
	if (msg == "UNDER ATTACK")
	{
		currState = CHASE;
	}
	if (msg == "UNHARMED")
	{
		currState = RETURN;
		CheckHP();
	}
}

void Guardian::Returnposition(double dt)
{
	if (pos != OriginalPosition)
	{
		vel = (OriginalPosition - pos).Normalize() * Speed;
		direction = vel;
		this->normal = direction.Normalized();
	}
}

void Guardian::CheckHP()
{
	if (health == maxhealth)
	{
		SendMessage("UNINJURED");
	}
	if (health != maxhealth)
	{
		SendMessage("INJURED");
	}
}

void Guardian::GoToPriest(double dt)
{
	distancefromPriest = DistBetween(_Priest->pos, this->pos);
	if (distancefromPriest > 15.f)
	{
		vel = (_Priest->pos - pos).Normalize() * Speed;
		direction = vel;
		this->normal = direction.Normalized();
	}
	else if (distancefromPriest < 15.f)
	{

		LocateTarget();
	}
}

void Guardian::LocateTarget()
{
	nearestTarget = nullptr;
	float nearestdistance = 100.f;
	
	for (vector<GameObject*>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it) 
	{
		if ((*it)->type == GO_ENEMY) {
			if (((*it)->pos - pos).Length() < nearestdistance) {
				nearestTarget = (*it);
				nearestdistance = (nearestTarget->pos - pos).Length();
			}
		}
	}
	if (nearestTarget)
	{
		_target = nearestTarget;
		ChaseTarget(_target);
		SetTarget(_target);
	}

	
}

void Guardian::ChaseTarget(GameObject *target)
{
	float distancefromtarget = DistBetween(target->pos, this->pos);
	if (distancefromtarget > 4.0f)
	{
		vel = (target->pos - pos).Normalize() * Speed;
		direction = vel;
		this->normal = direction.Normalized();
	}
	else
	{
		currState = ATTACK;
	}
}

bool Guardian::InAggroRange()
{
	for (std::vector<GameObject *>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->type == GameObject::GO_PRIEST || go->type == GameObject::GO_GUARDIAN)
			continue;
		if ((go->pos - pos).Length() <= Aggrorange) 
		{
			return true;
		}
	}
	return false;
}