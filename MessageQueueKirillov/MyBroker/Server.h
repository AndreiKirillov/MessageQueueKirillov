#pragma once
#include "framework.h"
#include "Message.h"
#include "MessageHandler.h"
#include "Connection.h"
#include "Broker.h"
#include <afxsock.h>

// Класс сервера, который будет обрабатывать подключения клиентов
class Server
{
private:
	CSocket _server;

	// непосредственно брокер сообщений, который будет распределять сообщения от клиентов
	Broker _message_broker;

	void ProcessClient(SOCKET hSock);
public:
	Server();
	~Server();

	bool StartUp();

	void WaitForConnection();
};

