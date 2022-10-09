#include "pch.h"
#include "Server.h"

extern std::mutex console_mtx;

Server::Server():_server(), _connections()
{
}

Server::~Server()
{
}

bool Server::StartUp()
{
    AfxSocketInit();
    if (_server.Create(12345))
        return true;
    else
        return false;
}

void Server::ProcessClient(SOCKET hSock, std::promise<std::string>&& promise_for_id)
{
    CSocket client_sock;
    client_sock.Attach(hSock);

    // читаем первое сообщение, из него узнаем никнейм нового клиента
    Message registration_message = Message::read(client_sock);
    if (MessageHandler::isRegistrationMessage(registration_message))
    {
        promise_for_id.set_value(registration_message.getSender());  // посылаем имя через promise в ожидающий future объект

        ////////////  оправить брокеру инфу о новом клиенте ///////////////////////////////////
    }

    // отсылаем подтверждение

    // в бесконечном цикле ждём новое сообщение
    
    // полученное сообщение передаем брокеру, он будет разбираться что делать
}

void Server::WaitForConnection()
{
    if (!_server.Listen())
    {
        std::lock_guard<std::mutex> console_lock(console_mtx);
        std::cout << "Server error connecting to client!" << std::endl;
        return;
    }
    CSocket client_sock;

    if (!_server.Accept(client_sock))
    {
        std::lock_guard<std::mutex> console_lock(console_mtx);
        std::cout << "Server error connecting to client!" << std::endl;
        return;
    }
    else
    {
        // создаем промис, в который запишем имя клента, когда узнаем его
        std::promise<std::string> promise_for_client_id;
        std::future<std::string> wait_for_promise = promise_for_client_id.get_future();

        // Создаём новое соединение, отправляем туда промис
        // В конструктор передаем функцию, которая будет обрабатывать взаимодействие с клиентом
        auto new_connection = std::make_unique<Connection>(&Server::ProcessClient, this, client_sock.Detach(), std::move(promise_for_client_id));

        // получаем обещанный id клиента, добавляем сведения о нём
        std::string client_id = wait_for_promise.get();
        new_connection->setId(client_id);

        std::lock_guard<std::mutex> console_lock(console_mtx);
        std::cout << "New client \"" << client_id << "\" connected to the server!" << std::endl;
    }
}

int Server::GetClientsCount()
{
    return 0;
}
