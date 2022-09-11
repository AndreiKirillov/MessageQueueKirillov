#pragma once
#include "framework.h"
//#include "Kirillov_lab1_cpp.h"
//#include "ThreadStorage.h"
//#include "Connection.h"
#include <afxsock.h>

// ����� �������, ������� ����� ������������ ����������� ��������
class Server
{
private:
	CSocket _server;

	//set<unique_ptr<Connection>> _connections;
	//mutex mtx_for_working_threads;             // �������������� ������ � ������� �������


	void ProcessClient(SOCKET hSock, int client_id);
	void CloseClient(int client_id);
public:
	Server();
	~Server();

	bool StartUp();

	void WaitForConnection();

	int GetClientsCount();
};

