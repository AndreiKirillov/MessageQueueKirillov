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

    // ������ ������ ���������, �� ���� ������ ������� ������ �������
    Message registration_message = Message::read(client_sock);
    if (registration_message.isRegistrationMessage())
    {
        std::string new_client_id = registration_message.getSender();
        promise_for_id.set_value(new_client_id);  // �������� ��� ����� promise � ��������� future ������

        // ��� ������� ������ � ����� �������
        _message_broker.addClient(new_client_id);
    }
    else   // ���� ������ �� �������� ��� ���, ��������� ���������� � ������
    {
        promise_for_id.set_exception(std::make_exception_ptr(std::runtime_error("Cant registrate new client")));
        return;
    }

    // �������� �������������

    // � ����������� ����� ��� ����� ���������
    
    // ���������� ��������� �������� �������, �� ����� ����������� ��� ������
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
        // ������� ������, � ������� ������� ��� ������, ����� ������ ���
        std::promise<std::string> promise_for_client_id;
        std::future<std::string> wait_for_promise = promise_for_client_id.get_future();

        // ������ ����� ����������, ���������� ���� ������
        // � ����������� �������� �������, ������� ����� ������������ �������������� � ��������
        auto new_connection = std::make_unique<Connection>(&Server::ProcessClient, this, client_sock.Detach(), std::move(promise_for_client_id));

        // �������� ��������� id �������, ��������� �������� � ��
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
