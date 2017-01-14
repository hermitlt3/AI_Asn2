#include "Messager.h"
#include "MessageBoard.h"

Messager::Messager()
{
	mb = MessageBoard::GetInstance();
	mb->AddMessager(this);
}

Messager::~Messager()
{
	
}

bool Messager::SendMessage(const std::string& msg)
{
	mb->BroadcastMessage(msg);
	return true;
}