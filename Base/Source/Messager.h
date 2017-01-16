/// @brief 
///
/// A messager class which will be in charged of sending messages
/// to the message board. Classes which wants to access the message
/// board will inherit from this class.
///

#ifndef MESSAGER_H_
#define MESSAGER_H_

#include <string>

class MessageBoard;
class Messager
{
public:
	// constructor
	Messager();
	// destructor
	~Messager();

	// Sends a message to the message board
	bool SendMessage(const std::string& msg);
	// Function to do things from to msg received
	virtual void OnNotification(const std::string& msg) = 0;
	// Update
	virtual void Update(double dt) = 0;

private:
	// The message board
	MessageBoard* mb;
};

#endif