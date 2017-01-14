#include "Priest.h"

Priest::Priest() :
currState(IDLE),
innerProximity(5.f),
outerProximity(10.f)
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

}

void Priest::FSM()
{
	int nextHealth = health;
	if (health <= 0)
		currState = DIE;

	// If health decreases due to enemy
	if (health != nextHealth) {
		currState = RUN;
	}
}