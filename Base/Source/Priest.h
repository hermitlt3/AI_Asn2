#ifndef PRIEST_H_
#define PRIEST_H_

#include "GameObject.h"
#include "Messager.h"

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
	
private:
	// Function to do things from to msg received
	virtual void OnNotification(const std::string& msg);
	// Current state of priest
	PRIEST_STATES currState;
	// Inner proximity
	float innerProximity;
	// Outer proximity
	float outerProximity;
};

#endif