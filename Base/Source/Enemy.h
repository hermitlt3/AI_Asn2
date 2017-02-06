#pragma once

#include "GameObject.h"
#include "Messager.h"

// Enemy is to demostrate collab AI and 
// the AI for the other two classes. There will be hiding 
// enemies that will reveal when message is received. 

class Enemy : public GameObject, public Messager
{
public:
	enum ENEMY_STATES
	{
		IDLE,
		CHASE,
		ATTACK,
		DIE
	};

	// Constructor
	Enemy();
	// Destructor
	~Enemy();
	// Update
	virtual void Update(double dt);
	// Set isLeader
	inline void SetIsLeader(const bool& set) { isLeader = set; }
	// Set speed
	inline void SetSpeed(const float& spd) { speed = spd; }
	// Function to do things from to msg received
	virtual void OnNotification(const std::string& msg);
	// Finite state machine
	void FSM();

	std::string GetState() {
		switch (currState) {
		case 0:
			return "IDLE";
		case 1:
			return "CHASE";
		case 2:
			return "ATTACK";
		case 3:
			return "DIE";
		}
		return "";
	}

private:	
	// Check if it is the main enemy
	bool isLeader;
	// Message sent once
	bool isSent;
	// Speed of enemy
	float speed;
	// Target
	GameObject* target;
	// Personal timer
	double timer;
	// Enemy states
	ENEMY_STATES currState;
	// Function to chase friendly
	void ChaseFriendly();
	// Function to attack target
	void AttackTarget(double dt);
};