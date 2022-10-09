#pragma once
#include "framework.h"
#include "threadsafe_queue.h"
#include "Message.h"
#include "Client.h"

class Broker
{
private:
	threadsafe_queue<std::shared_ptr<Message>> _incoming_messages;   // очередь поступаемых на сервер сообщений

	std::map<std::string, Client> _clients;     // набор активных клиентов сервера
public:
	Broker();

	void addClient(const std::string& client_id);
};

