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
std::string username;
bool is_connected_to_server = false;

void printMenu()
{
    std::cout << "Что сделать дальше?" << std::endl;
    std::cout << "1 - Написать сообщение" << std::endl;
    std::cout << "2 - Отключиться от сервера" << std::endl;
}

enum class Action
{
    SendDirectMessage = 1, ExitServer = 2
};

bool registrateToServer(const std::string& username)
{
    AfxSocketInit();

    if (Message::sendRegistrationRequest(username))
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

void checkIncomingMessages(const std::string& username)
{
    while (true)
    {
        if (is_connected_to_server)
        {
            Message check_message = Message::sendDataRequest(username);
            switch (check_message.getHeader().type)
            {
            case MessageType::Empty:
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                break;
            }
            default:
            {
                //std::lock_guard<std::mutex> console_lock(console_mtx);
                std::cout << "Message from " << check_message.getSender() << ": \"" << check_message.getData() << "\"" << std::endl;
                break;
            }
            }
        }
        else 
            return;
    }
}

void sendDirectMessage()
{
    //std::unique_lock<std::mutex> console_lock(console_mtx);
    std::cout << "Кому написать?" << std::endl;
    //console_lock.unlock();
    std::string recipient;
    std::cin >> recipient;

    //console_lock.lock();
    std::cout << "Введите текст сообщения" << std::endl;
    //console_lock.unlock();
    std::string data;
    std::cin >> data;//.clear();
    //getline(std::cin, data);

    MessageHeader header;
    header.type = MessageType::Peer2Peer;
    header.size = data.size();
    Message message(header);
    message.setSender(username);
    message.setRecipient(recipient);
    message.setData(data);

    CSocket server_sock;
    server_sock.Create();
    if (server_sock.Connect("127.0.0.1", 12345))
    {
        Message::send(server_sock, message);

        if (Message::waitConfirm(server_sock))
        {
            //console_lock.lock();
            std::cout << "Отправлено!" << std::endl;
        }
        else
        {
            //console_lock.lock();
            std::cout << "Сервер сообщил об ошибке в отправленном сообщении" << std::endl;
        }
    }
}

bool exitServer()
{
    MessageHeader header;
    header.type = MessageType::Exit;
    header.size = 0;
    header.recipient = MessageClient::Broker;
    Message message(header);
    message.setSender(username);

    CSocket server_sock;
    server_sock.Create();
    if (server_sock.Connect("127.0.0.1", 12345))
    {
        Message::send(server_sock, message);

        if (Message::waitConfirm(server_sock))
        {
            //console_lock.lock();
            is_connected_to_server = false;
            std::cout << "Вы отключились от сервера!" << std::endl;
            return true;
        }
        else
        {
            //console_lock.lock();
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
                        sendDirectMessage();
                        break;
                    }
                    case (int)Action::ExitServer:
                    {
                        if (exitServer())
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
