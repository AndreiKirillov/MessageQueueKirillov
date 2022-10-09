#include "pch.h"
#include "Broker.h"

Broker::Broker(): _incoming_messages(), _clients()
{
}

void Broker::addClient(const std::string& client_id)
{
	_clients.emplace(client_id, Client(client_id));   // добавляем нового клиента
}
