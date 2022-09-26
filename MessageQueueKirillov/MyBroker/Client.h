#pragma once
#include "framework.h"
#include "threadsafe_queue.h"
#include "Message.h"
#include <condition_variable>

using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;

class Client
{
private:
	std::string _id_nickname;
	threadsafe_queue<Message> _message_queue;
	std::condition_variable _event;
	CSocket _connection_sock;
	TimePoint _last_action_time;
public:
	Client();
};

