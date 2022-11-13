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

	// ��������������� ������ ���������, ������� ����� ������������ ��������� �� ��������
	Broker _message_broker;

	void ProcessClient(SOCKET hSock);
public:
	Server();
	~Server();

	bool StartUp();

	void WaitForConnection();
};

