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
	enum GUARDIAN_STATES
	{
		IDLE = 0,
		CHASE,
		RETURN,
		ATTACK,
		BEING_HEALED,
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

	float gethealth();
	std::string GetState() {
		switch (currState) {
		case 0:
			return "IDLE";
		case 1:
			return "CHASE";
		case 2:
			return "RETURN";
		case 3:
			return "ATTACK";
		case 4:
			return "RECOVERING";
		case 5:
			return "DIE";
		}
	}

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

	float nexthealth;
	float Speed;
	Vector3 direction;
	float Aggrorange;
	float distancefromPriest;
	float distancefromoriginalposition;
};

static float DistBetween(const Vector3& posOne, const Vector3& posTwo)
{
	return (posOne - posTwo).Length();
}