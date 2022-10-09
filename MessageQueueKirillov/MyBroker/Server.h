#pragma once
#include "framework.h"
#include "Message.h"
#include "MessageHandler.h"
//#include "Kirillov_lab1_cpp.h"
//#include "ThreadStorage.h"
#include "Connection.h"
#include <afxsock.h>

// Класс сервера, который будет обрабатывать подключения клиентов
class Server
{
private:
	CSocket _server;

	// коллекция всех подсоединений к серверу
	std::set<std::unique_ptr<Connection>> _connections;

	void ProcessClient(SOCKET hSock, std::promise<std::string>&& promise_for_id);
	void RegisterClient();
	void CloseClient(int client_id);
public:
	Server();
	~Server();

	bool StartUp();

	void WaitForConnection();

	int GetClientsCount();
};

