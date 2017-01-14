#ifndef MESSAGE_BOARD_H
#define MESSAGE_BOARD_H

#include "SingletonTemplate.h"
#include <string>
#include <list>
#include <deque>

class Messager;
class MessageBoard : public Singleton<MessageBoard>
{
	friend Singleton<MessageBoard>;

public:
	// Constructor
	MessageBoard();
	// Destructor
	~MessageBoard();

	// Adds a new item into the list of Messagers
	bool AddMessager(Messager* msgr);
	// Broadcast message to all messagers
	void BroadcastMessage(const std::string& msg);
	// Update
	void Update(double dt);
	// Gets queue 
	std::deque<std::string> GetList() { return latestMessage; }

private:
	// List of messagers
	std::list<Messager*> messagerList;
	// Queue of messages being passed in
	std::deque<std::string> latestMessage;
	// Timer for it to display
	double timer;
};

#endif