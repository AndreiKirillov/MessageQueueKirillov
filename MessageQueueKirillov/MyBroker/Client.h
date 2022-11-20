#pragma once
#include "framework.h"
#include "threadsafe_queue.h"
#include "Message.h"
#include <condition_variable>

class Client
{
private:
	std::string _id;
	threadsafe_queue<Message> _message_queue;
	std::chrono::steady_clock::time_point _last_action_time;
public:
	Client();
	Client(const std::string& id);
	Client(SOCKET hSock);

	void addMessage(const Message& message);

	Message getMessage();

	std::chrono::steady_clock::time_point getLastActionTime() const;

	bool hasMessages();

	void refreshActivity();
};

