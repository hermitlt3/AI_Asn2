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
currState(IDLE), Speed(10.0f), Aggrorange(5.f)
{
	health = 100;
	maxhealth = health;
	nexthealth = health;
	originalScale = 5;
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
			if (!InAggroRange())
			{
				currState = RETURN;
			}
			break;
		}
		case RETURN:
		{
			if (InAggroRange())
			{
				LocateTarget();
			}

			break;
		}
		case BEING_HEALED:
		{
			if (health == maxhealth)
			{
				SendMessage("UNINJURED");
				currState = IDLE;
			}
		}
		case IDLE:
		{
			if (health != nexthealth)
			{
				LocateTarget();
			}
			
		}
		}
	}
}

void Guardian::Update(double dt)
{
	//cout << health << endl;
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
		this->normal = (_target->pos - this->pos).Normalized();
		timer += dt;
		if (timer > 1.f)
		{
			_target->health -= (float)Math::RandFloatMinMax(10.f, 15.f);
			timer = 0;
		}
		break;
	}
	case BEING_HEALED:
	{
		break;
	}
	case DIE:
	{
				if (active) {
					scale.x -= dt * 3; scale.y -= dt * 3; scale.z -= dt * 3;
					if (scale.x <= Math::EPSILON) {
						SendMessage("GUARDIAN DOWN");
						active = false;
						scale.Set(originalScale, originalScale, originalScale);
					}
				}
	}
	}
	nexthealth = health;
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
	}
	if (msg == "REVIVING")
	{
		currState = IDLE;
		active = true;
		health = 100;
	}
}

void Guardian::Returnposition(double dt)
{
	distancefromoriginalposition = DistBetween(OriginalPosition,this->pos);
	if (distancefromoriginalposition > 0.1f)
	{
		vel = (this->OriginalPosition - this->pos).Normalize() * Speed;
		direction = vel;
		this->normal = direction.Normalized();
	}
	else
	{
		this->vel.SetZero();
		currState = BEING_HEALED;
		CheckHP();
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
	if (distancefromPriest > 20.f)
	{
		vel = (_Priest->pos - pos).Normalize() * Speed;
		direction = vel;
		this->normal = direction.Normalized();
	}
	else if (distancefromPriest < 20.f)
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

float Guardian::gethealth()
{
	return health;
}