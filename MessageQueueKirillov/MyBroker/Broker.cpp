#include "pch.h"
#include "Broker.h"

#define CONFIRM true;
#define ERROR false

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

        std::unique_lock<std::mutex> clients_lock(_mtx_clients);
        if (!clientExists(new_client_id))  // если нет пользователя с таким именем
        {
            addClient(new_client_id);
            clients_lock.unlock();

            std::lock_guard<std::mutex> console_lock(console_mtx);
            std::cout << "New client \"" << new_client_id << "\" connected to the server!" << std::endl;
            return CONFIRM;
        }
        else
            return ERROR;  // клиент с таким именем существует, сообщаем об ошибке
    }

    case MessageType::GetData:
    {

    }

    case MessageType::Peer2Peer:      // сообщение от клиента клиенту
    {
        std::string sender_name = message.getSender();
        std::string recipient_name = message.getRecipient();

        std::lock_guard<std::mutex> clients_lock(_mtx_clients);
        if (clientExists(sender_name) && clientExists(recipient_name))
        {
            if (sender_name != recipient_name)
            {
                auto recipient = _clients.find(recipient_name);
                recipient->second->addMessage(message);
                return CONFIRM;
            }
            else
                return ERROR;
        }
        else
            return ERROR;
    }

    case MessageType::Exit:
    {
        std::string client = message.getSender();

        std::unique_lock<std::mutex> clients_lock(_mtx_clients);
        if (clientExists(client))
        {
            _clients.erase(client);
            clients_lock.unlock();

            std::lock_guard<std::mutex> console_lock(console_mtx);
            std::cout << "Client " << client << " disconnected from the server!" << std::endl;
            return CONFIRM;
        }
        else
            return ERROR;
    }
    }
}
