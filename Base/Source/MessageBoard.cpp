#include "MessageBoard.h"
#include "Messager.h"

MessageBoard::MessageBoard()
{
	// Clears the list
	messagerList.clear();
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
	// Iterate through every element to notify them
	for (std::list<Messager*>::iterator it = messagerList.begin();
		it != messagerList.end(); ++it)
	{
		// Do things in their respective onNotification(string)
		(*it)->OnNotification(msg);
	}
}
