#ifndef MESSAGE_BOARD_H
#define MESSAGE_BOARD_H

#include "SingletonTemplate.h"
#include <string>
#include <list>

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

private:
	// List of messagers
	std::list<Messager*> messagerList;
};

#endif