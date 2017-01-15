#include "Guardian.h"
#include "Priest.h"
#include "Enemy.h"
#include "GameObjectManager.h"
#include <vector>
#include <iostream>
using std::cout;
using std::endl;

Guardian::Guardian() :
timer(0.0),
currState(RETURN), Speed(4.0f), Aggrorange(2.f)
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
			if (!InAggroRange())
			{
				currState = RETURN;
			}
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
	if (_target == nullptr)
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
	
		break;
	}
	case RETURN:
	{
		Returnposition(dt);
		cout << "Returning" << endl;
		break;
	}
	case ATTACK:
	{
		cout << "Attacking" << endl;
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
		LocateTarget();
		currState = CHASE;
	}
	if (msg == "UNHARMED")
	{
		currState = RETURN;
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

void Guardian::LocateTarget()
{
	
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