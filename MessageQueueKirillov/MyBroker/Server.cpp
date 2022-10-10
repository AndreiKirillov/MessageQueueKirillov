#include "pch.h"
#include "Server.h"

extern std::mutex console_mtx;

Server::Server():_server(), _connections(), _message_broker()
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
    if (registration_message.isRegistrationMessage())
    {
        std::string new_client_id = registration_message.getSender();
        promise_for_id.set_value(new_client_id);  // посылаем имя через promise в ожидающий future объект

        // даём брокеру данные о новом клиенте
        _message_broker.addClient(new_client_id);
    }
    else   // если клиент не сообщает своё имя, сохраняем исключение в промис
    {
        promise_for_id.set_exception(std::make_exception_ptr(std::runtime_error("Cant registrate new client")));
        return;
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
        try
        {
            std::string client_id = wait_for_promise.get();
            new_connection->setId(client_id);

            std::lock_guard<std::mutex> console_lock(console_mtx);
            std::cout << "New client \"" << client_id << "\" connected to the server!" << std::endl;
        }
        catch(std::runtime_error& ex)
        {
            std::lock_guard<std::mutex> console_lock(console_mtx);
            std::cout << "Server error! " << ex.what() << std::endl;
        }
    }
}

int Server::GetClientsCount()
{
    return 0;
}
