#include "pch.h"
#include "Broker.h"

extern std::mutex console_mtx;

Broker::Broker(): _clients(), _mtx_clients()
{
}

void Broker::addClient(const std::string& id)  // добавление клиента в контейнер активных клиентов
{
    _clients.emplace(id, std::make_unique<Client>(id));
}

bool Broker::clientExists(const std::string& client_id)
{
	std::lock_guard<std::mutex> clients_lock(_mtx_clients);
	return _clients.contains(client_id);
}

bool Broker::processMessage(const Message& message)
{
    MessageHeader header = message.getHeader();
    switch (header.type)
    {
    case MessageType::Registration:
    {
        std::string new_client_id = message.getSender();

        if (!clientExists(new_client_id))  // если нет пользователя с таким именем
        {
            addClient(new_client_id);
            std::lock_guard<std::mutex> console_lock(console_mtx);
            std::cout << "New client \"" << new_client_id << "\" connected to the server!" << std::endl;
            return true;
        }
        else
            return false;  // клиент с таким именем существует, сообщаем об ошибке
    }
    }
}
