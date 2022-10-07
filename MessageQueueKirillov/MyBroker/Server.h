#pragma once
#include "framework.h"
#include "Message.h"
//#include "Kirillov_lab1_cpp.h"
//#include "ThreadStorage.h"
#include "Connection.h"
#include <afxsock.h>

// ����� �������, ������� ����� ������������ ����������� ��������
class Server
{
private:
	CSocket _server;

	// ��������� ���� ������������� � �������
	std::set<std::unique_ptr<Connection>> _connections;
	//mutex mtx_for_working_threads;             // �������������� ������ � ������� �������

	void ProcessClient(SOCKET hSock, std::promise<std::string>&& promise_for_id);
	void RegisterClient()
	void CloseClient(int client_id);
public:
	Server();
	~Server();

	bool StartUp();

	void WaitForConnection();

	int GetClientsCount();
};

