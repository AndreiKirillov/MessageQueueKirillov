#include "pch.h"
#include "Server.h"

extern std::mutex console_mtx;

Server::Server():_server(), _message_broker()
{
}

Server::~Server()
{
}

bool Server::StartUp()
{
    AfxSocketInit();
    if (_server.Create(12345))
    {
        return true;
    }
    else
        return false;
}

void Server::ProcessClient(SOCKET hSock)
{
    CSocket client_sock;
    client_sock.Attach(hSock);

    Message message_from_client = Message::read(client_sock);
    _message_broker.processMessage(message_from_client, client_sock);  // ?????? ???????????? ?????????
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
        // ????????? ????????? ??????? ? ????? ??????
        std::thread thread_to_process_new_client(&Server::ProcessClient, this, client_sock.Detach());
        thread_to_process_new_client.detach();
    }
}
