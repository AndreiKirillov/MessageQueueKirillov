#pragma once
#include "framework.h"
#include "threadsafe_queue.h"
#include "Message.h"
#include "Client.h"

class Broker
{
private:
	std::map<std::string, std::unique_ptr<Client>> _clients;     // набор активных клиентов сервера
	std::mutex _mtx_clients;
public:
	Broker();

	void addClient(const std::string& id);
	bool clientExists(const std::string& client_id);

	bool processMessage(const Message& message);
};

