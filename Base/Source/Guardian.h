#pragma once

#include "GameObject.h"
#include "Messager.h"

class Guardian : public GameObject, public Messager
{
public:
	// States
	enum GUARDIAN_STATES
	{
		IDLE = 0,
		CHASE,
		RETURN,
		ATTACK,
		DIE
	};
	// Constructor
	Guardian();
	// Destructor
	~Guardian();
	// Update
	virtual void Update(double dt);
	// Finite State Machine
	void FSM();

private:
	// Function to do things from to msg received
	virtual void OnNotification(const std::string& msg);
	// Current state of priest
	GUARDIAN_STATES currState;
	// Personal timer
	double timer;
};