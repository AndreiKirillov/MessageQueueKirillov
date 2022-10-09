#pragma once
#include "framework.h"
#include "Message.h"
#include "MessageHandler.h"
#include "Connection.h"
#include "Broker.h"
#include <afxsock.h>

// ����� �������, ������� ����� ������������ ����������� ��������
class Server
{
private:
	CSocket _server;

	// ��������� ���� ������������� � �������
	std::set<std::unique_ptr<Connection>> _connections;

	// ��������������� ������ ���������, ������� ����� ������������ ��������� �� ��������
	Broker _message_broker;

	void ProcessClient(SOCKET hSock, std::promise<std::string>&& promise_for_id);
	void CloseClient(int client_id);
public:
	Server();
	~Server();

	bool StartUp();

	void WaitForConnection();

	int GetClientsCount();
};

