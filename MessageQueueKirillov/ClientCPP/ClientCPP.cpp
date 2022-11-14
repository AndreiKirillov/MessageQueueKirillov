// ClientCPP.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "ClientCPP.h"

#include "../MyBroker/Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Единственный объект приложения

CWinApp theApp;

using namespace std;

void registrateToServer()
{
    AfxSocketInit();
    std::cout << "Введите имя для подключения к серверу" << std::endl;
    std::string username;
    std::cin >> username;

    if(Message::sendRegistrationRequest(username))
        std::cout << "Регистрация проведена успешно!" << std::endl;
    else
        std::cout << "Не удалось зарегистрироваться!" << std::endl;

}

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            setlocale(LC_ALL, "russian");
            // TODO: вставьте сюда код для приложения.
            registrateToServer();
            int a;
            std::cin >> a;
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}
