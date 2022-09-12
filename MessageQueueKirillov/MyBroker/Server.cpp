#include "pch.h"
#include "Server.h"

extern std::mutex console_mtx;

Server::Server():_server()//, _connections(), _working_threads()
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

void Server::WaitForConnection()
{
    if (!_server.Listen())
    {
        std::lock_guard<std::mutex> console_lock(console_mtx);
        std::cout << "Server error connecting to client!" << std::endl;
        return;
    }
    CSocket client;

    if (!_server.Accept(client))
    {
        std::lock_guard<std::mutex> console_lock(console_mtx);
        std::cout << "Server error connecting to client!" << std::endl;
        return;
    }
    else
    {
        //auto new_connection = std::make_unique<Connection>();                                  // Создаём новое соединение
        //ProcessClient(client.Detach(), new_connection->GetID());
        //thread t = thread(&Server::ProcessClient, this, client.Detach(), new_connection->GetID());
        //t.join();
        //new_connection->Start(&Server::ProcessClient, this, client.Detach(), new_connection->GetID());    // его обработка запустится в отдельном потоке 
        //_connections.insert(std::move(new_connection));
    }
}

int Server::GetClientsCount()
{
    return 0;
}
