#ifndef PRIEST_H_
#define PRIEST_H_

#include "GameObject.h"
#include "Messager.h"

class Guardian;
class Priest : public Messager, public GameObject
{
public:
	// States
	enum PRIEST_STATES
	{
		IDLE = 0,
		HEAL,
		RUN,
		DIE
	};
	// Constructor
	Priest();
	// Destructor
	~Priest();
	// Update
	virtual void Update(double dt);
	// Finite State Machine
	void FSM();
	// Set guardian
	inline void SetGuardian(Guardian* _guardian) { guardian = _guardian; }

	std::string GetState() {
		switch (currState) {
		case 0:
			return "IDLE";
		case 1:
			return "HEAL";
		case 2:
			return "RUN";
		case 3:
			return "DIE";
		}
	}

private:
	// Function to do things from to msg received
	virtual void OnNotification(const std::string& msg);
	// Current state of priest
	PRIEST_STATES currState;
	// Inner proximity
	float innerProximity;
	// Outer proximity
	float outerProximity;
	// Health to check if it is hit, always updated to the health in GameObject class
	int nextHealth;
	// Personal timer
	double timer;
	// float
	float speed;

	// Find if there are enemies within outer proximity, and return a Vector3 which is the flocking distance
	Vector3 IsEnemiesInOuterP();
	// Find if there are enemies within inner proximity, and return if there are enemies
	bool IsEnemiesInInnerP();
	// Finds and heals guardian
	Guardian* guardian;
	void HealsGuardian(double dt);
	// Returns to idle state
	void ReturnToIdle(double dt);
};

#endif