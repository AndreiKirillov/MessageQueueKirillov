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

	void processMessage(const Message& message, CSocket& client_sock);

	void checkInactiveClients(std::chrono::seconds timeout_limit);
};

