#include "pch.h"
#include "Client.h"

Client::Client():_id(), _message_queue(), _last_action_time(std::chrono::steady_clock::now())
{
}

Client::Client(const std::string& id):_id(id), _message_queue(), _last_action_time(std::chrono::steady_clock::now())
{
}

void Client::addMessage(const Message& message)
{
	_message_queue.push(message);
}

Message Client::getMessage()
{
	auto msg_ptr = _message_queue.try_pop();
	if (msg_ptr != nullptr)
		return *msg_ptr;
	
	return Message();
}

std::chrono::steady_clock::time_point Client::getLastActionTime() const
{
	return _last_action_time;
}

bool Client::hasMessages()
{
	return !_message_queue.empty();
}

void Client::refreshActivity()
{
	_last_action_time = std::chrono::steady_clock::now();
}
