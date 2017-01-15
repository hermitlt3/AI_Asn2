#pragma once

#include "GameObject.h"
#include "Messager.h"

// Enemy is to demostrate collab AI and 
// the AI for the other two classes. There will be hiding 
// enemies that will reveal when message is received. 

class Enemy : public GameObject, public Messager
{
public:
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

private:	
	// Check if it is the main enemy
	bool isLeader;
	// Message sent once
	bool isSent;
	// Speed of enemy
	float speed;
};