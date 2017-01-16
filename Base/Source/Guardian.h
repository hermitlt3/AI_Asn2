#pragma once

#include "GameObject.h"
#include "Messager.h"
#include "Priest.h"

class Priest;
class Enemy;
class Guardian : public GameObject, public Messager
{
public:
	// States

	//New State Types
	enum GUARDIAN_SCENARIO
	{
		FSM_HELPING_PRIEST = 0,
		FSM_DEFENDING,
		FSM_DEAD,
		TOTAL,
	};
	enum HELPING_STATES
	{
		HELP_IDLE = 0,
		HELP_CHASE,
		HELP_ATTACK,
		HELP_RETURN,
		TOTAL_HELP,
	};
	enum DEFENDING_STATES
	{
		DEFEND_IDLE = 0,
		DEFEND_ATTACK,
		DEFEND_RETURN,
		TOTAL_DEFEND,
	};

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
	inline void SetPriest(Priest* Priest){ _Priest = Priest; };
	inline void SetTarget(GameObject* thetarget){ _target = thetarget; };
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
	Priest *_Priest;
	GameObject *_target;
	GameObject *nearestTarget;

	//This is called when the Guardian is in Return State;
	void Returnposition(double dt);
	void GoToPriest(double dt);
	void LocateTarget();
	void ChaseTarget(GameObject *target);
	void CheckHP();
	bool InAggroRange();

	float Speed;
	Vector3 direction;
	float Aggrorange;
	float distancefromPriest;
};

static float DistBetween(const Vector3& posOne, const Vector3& posTwo)
{
	return (posOne - posTwo).Length();
}