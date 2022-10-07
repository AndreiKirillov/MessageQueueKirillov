#pragma once
#include "framework.h"
#include "threadsafe_queue.h"
#include "Message.h"
#include <condition_variable>

using TimePoint = std::chrono::steady_clock::time_point;

class Client
{
private:
	std::string _id_nickname;
	threadsafe_queue<std::shared_ptr<Message>> _message_queue;
	std::condition_variable _event;
	CSocket _connection_sock;
	TimePoint _last_action_time;
public:
	Client();

	void addMessage(const std::shared_ptr<Message>& message);

	bool hasMessages();
};

