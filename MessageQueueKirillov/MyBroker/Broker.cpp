#include "pch.h"
#include "Broker.h"

extern std::mutex console_mtx;

Broker::Broker(): _clients(), _mtx_clients()
{
    std::thread thread_for_finding_inactive_clients(&Broker::checkInactiveClients, this, std::chrono::seconds(2));
    thread_for_finding_inactive_clients.detach();
}

void Broker::addClient(const std::string& id)  // добавление клиента в контейнер активных клиентов
{
    _clients.emplace(id, std::make_unique<Client>(id));
}

bool Broker::clientExists(const std::string& client_id)
{
	return _clients.contains(client_id);
}

void Broker::processMessage(const Message& message, CSocket& client_sock)
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
            Message::sendConfirm(client_sock);
        }
        else
            Message::sendError(client_sock);  // клиент с таким именем существует, сообщаем об ошибке
        break;
    }

    case MessageType::GetData:
    {
        std::string sender_name = message.getSender();
        auto client = _clients.find(message.getSender());
        if (client != _clients.end())
        {
            client->second->refreshActivity();
            if (client->second->hasMessages())
            {
                Message m = client->second->getMessage();
                Message::send(client_sock, m);
            }
            else
                Message::sendEmpty(client_sock);
        }
        break;
    }

    case MessageType::Peer2Peer:      // сообщение от клиента клиенту
    {
        std::string sender_name = message.getSender();
        std::string recipient_name = message.getRecipient();

        std::lock_guard<std::mutex> clients_lock(_mtx_clients);
        if (clientExists(sender_name))
        {
            auto sender = _clients.find(sender_name);
            sender->second->refreshActivity();
            if (clientExists(recipient_name) && sender_name != recipient_name)
            {
                auto recipient = _clients.find(recipient_name);
                recipient->second->addMessage(message);
                Message::sendConfirm(client_sock);
            }
            else
                Message::sendError(client_sock);
        }
        else
            Message::sendError(client_sock);
        break;
    }

    case MessageType::Broadcast:
    {
        std::string sender_name = message.getSender();

        std::lock_guard<std::mutex> clients_lock(_mtx_clients);
        if (clientExists(sender_name))
        {
            auto sender = _clients.find(sender_name);
            sender->second->refreshActivity();
            for (auto& client : _clients)
            {
                if (sender_name != client.first)
                {
                    client.second->addMessage(message);
                }
            }
            Message::sendConfirm(client_sock);
        }
        else
            Message::sendError(client_sock);
        break;
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
            Message::sendConfirm(client_sock);
        }
        else
            Message::sendError(client_sock);
        break;
    }
    }
}

void Broker::checkInactiveClients(std::chrono::seconds timeout_limit)
{
    while (true)
    {
        std::unique_lock<std::mutex> clients_lock(_mtx_clients);
        if (_clients.size() > 0)
        {
            for (auto& client : _clients)
            {
                auto time_difference = std::chrono::steady_clock::now() - client.second->getLastActionTime();
                if (time_difference > timeout_limit)
                {
                    _clients.erase(client.first);
                }
            }
        }
        clients_lock.unlock();
        std::this_thread::sleep_for(timeout_limit);
    }
}
