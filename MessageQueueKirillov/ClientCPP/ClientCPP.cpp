// ClientCPP.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "ClientCPP.h"
#include <thread>
#include <mutex>

#include "../MyBroker/Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Единственный объект приложения

CWinApp theApp;

using namespace std;

std::mutex console_mtx;
bool is_connected_to_server = false;

void printMenu()
{
    std::cout << "Что сделать дальше?" << std::endl
              << "1 - Написать сообщение юзеру" << std::endl
              << "2 - Написать широковещательное сообщение" << std::endl
              << "3 - Отключиться от сервера" << std::endl;
}

enum class Action
{
    SendDirectMessage = 1, BroadcastMessage = 2, ExitServer = 3
};

bool registrateToServer(const std::string& _username)
{
    AfxSocketInit();

    if (Message::sendRegistrationRequest(_username))
    {
        is_connected_to_server = true;
        std::cout << "Регистрация проведена успешно!" << std::endl;
        return true;
    }
    else
    {
        std::cout << "Не удалось зарегистрироваться!" << std::endl;
        return false;
    }
}

void checkIncomingMessages(const std::string& _username)
{
    while (true)
    {
        if (is_connected_to_server)
        {
            Message check_message = Message::sendDataRequest(_username);
            switch (check_message.getHeader().type)
            {
            case MessageType::Empty:
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                break;
            }
            default:
            {
                std::cout << "Message from " << check_message.getSender() << ": \"" << check_message.getData() << "\"" << std::endl;
                break;
            }
            }
        }
        else 
            return;
    }
}

void sendDirectMessage(const std::string& from_username)
{
    std::cout << "Кому написать?" << std::endl;
    std::string recipient;
    std::cin >> recipient;

    std::cout << "Введите текст сообщения" << std::endl;
    std::string data;
    std::cin.ignore(32767, '\n');
    getline(std::cin, data);

    MessageHeader header;
    header.type = MessageType::Peer2Peer;
    Message message(header);
    message.setSender(from_username);
    message.setRecipient(recipient);
    message.setData(data);

    CSocket server_sock;
    server_sock.Create();
    if (server_sock.Connect("127.0.0.1", 12345))
    {
        Message::send(server_sock, message);

        if (Message::waitConfirm(server_sock))
        {
            std::cout << "Отправлено!" << std::endl;
        }
        else
        {
            std::cout << "Сервер сообщил об ошибке в отправленном сообщении" << std::endl;
        }
    }
}

void sendBroadcastMessage(const std::string& from_username)
{
    std::cout << "Введите текст сообщения" << std::endl;
    std::string data;
    std::cin.ignore(32767, '\n');
    getline(std::cin, data);

    MessageHeader header;
    header.type = MessageType::Broadcast;
    header.recipient = MessageClient::All;
    Message message(header);
    message.setSender(from_username);
    message.setData(data);

    CSocket server_sock;
    server_sock.Create();
    if (server_sock.Connect("127.0.0.1", 12345))
    {
        Message::send(server_sock, message);

        if (Message::waitConfirm(server_sock))
        {
            std::cout << "Отправлено!" << std::endl;
        }
        else
        {
            std::cout << "Сервер сообщил об ошибке в отправленном сообщении" << std::endl;
        }
    }
}

bool exitServer(const std::string& _username)
{
    MessageHeader header;
    header.type = MessageType::Exit;
    header.size = 0;
    header.recipient = MessageClient::Broker;
    Message message(header);
    message.setSender(_username);

    CSocket server_sock;
    server_sock.Create();
    if (server_sock.Connect("127.0.0.1", 12345))
    {
        Message::send(server_sock, message);

        if (Message::waitConfirm(server_sock))
        {
            is_connected_to_server = false;
            std::cout << "Вы отключились от сервера!" << std::endl;
            return true;
        }
        else
        {
            std::cout << "Сервер сообщил об ошибке!" << std::endl;
            return false;
        }
    }
    else
        return false;
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
            std::cout << "Введите имя для подключения к серверу" << std::endl;
            std::string username;
            std::cin >> username;
            if (registrateToServer(username))
            {
                std::thread receiving_thread(checkIncomingMessages, username);
                receiving_thread.detach();

                while (true)
                {
                    printMenu();
                    int user_action;
                    std::cin >> user_action;
                    switch (user_action)
                    {
                    case (int)Action::SendDirectMessage:
                    {
                        sendDirectMessage(username);
                        break;
                    }
                    case (int)Action::BroadcastMessage:
                    {
                        sendBroadcastMessage(username);
                        break;
                    }
                    case (int)Action::ExitServer:
                    {
                        if (exitServer(username))
                        {
                            int a;
                            std::cin >> a;
                            return 0;
                        }
                        else
                            break;
                    }
                    }
                }
            }
            else
            {
                int a;
                std::cin >> a;
                return 0;
            }
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
