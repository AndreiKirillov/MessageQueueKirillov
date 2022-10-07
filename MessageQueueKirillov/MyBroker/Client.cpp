#include "pch.h"
#include "Client.h"

Client::Client():_id_nickname(), _message_queue(), _event(), _connection_sock(),
_last_action_time(std::chrono::steady_clock::now())
{
}

void Client::addMessage(const std::shared_ptr<Message>& message)
{
	_message_queue.push(message);
}

bool Client::hasMessages()
{
	return _message_queue.empty();
}
