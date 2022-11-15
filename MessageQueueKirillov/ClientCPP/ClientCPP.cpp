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

bool registrateToServer(const std::string& username)
{
    AfxSocketInit();

    if (Message::sendRegistrationRequest(username))
    {
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
                    //std::unique_lock<std::mutex> console_lock(console_mtx);
                    std::cout << "Кому написать?" << std::endl;
                    //console_lock.unlock();
                    std::string recipient;
                    std::cin >> recipient;

                    //console_lock.lock();
                    std::cout << "Введите текст сообщения" << std::endl;
                    //console_lock.unlock();
                    std::string data;
                    std::cin >> data;

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
            }
            else
            {
                int a;
                std::cin >> a;
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
