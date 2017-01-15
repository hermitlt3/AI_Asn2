#pragma once

#include "GameObject.h"
#include "Messager.h"

class Priest;
class Enemy;
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

	void SetOriginalPosition(Vector3 position);
	Vector3 GetOriginalPosition();

private:
	// Function to do things from to msg received
	virtual void OnNotification(const std::string& msg);
	// Current state of priest
	GUARDIAN_STATES currState;
	// Personal timer
	double timer;
	//A value to store it's original position
	Vector3 OriginalPosition;

	//This is to Check if the priest is being attacked
	Priest* _Priest;
	Enemy* _target;

	//This is called when the Guardian is in Return State;
	void Returnposition(double dt);
	void LocateTarget();
	void CheckHP();
	bool InAggroRange();

	float Speed;
	Vector3 direction;
	float Aggrorange;
};