#include "MessageBoard.h"
#include "Messager.h"
#include <iostream>

MessageBoard::MessageBoard()
{
	// Clears the list
	messagerList.clear();
	timer = 0.0;
}

MessageBoard::~MessageBoard()
{

}

bool MessageBoard::AddMessager(Messager* msgr)
{
	// Pushes back a messager into the list
	messagerList.push_back(msgr);
	return true;
}

void MessageBoard::BroadcastMessage(const std::string& msg)
{
	// Add the message to the queue to display it
	latestMessage.push_back(msg);
	// Iterate through every element to notify them
	for (std::list<Messager*>::iterator it = messagerList.begin();
		it != messagerList.end(); ++it)
	{
		// Do things in their respective onNotification(string)
		(*it)->OnNotification(msg);
	}
}

void MessageBoard::Update(double dt)
{

	// If there is still a message being sent
	if (!latestMessage.empty())
	{
		// Timer start counting
		timer += dt;
		// If timer exceeds a certain limit
		if (timer > 3.0)
		{
			// Resets timer and pops queue
			latestMessage.pop_front();
			timer = 0.0;
		}
	}
}