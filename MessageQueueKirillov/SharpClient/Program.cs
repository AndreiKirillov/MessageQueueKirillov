using System;
using System.Threading;

namespace SharpClient
{
    class Program
    {
        static bool is_connected_to_server = false;
        enum Action
        {
            SendDirectMessage = 1, BroadcastMessage = 2, ExitServer = 3
        }

        static void PrintMenu()
        {
            Console.WriteLine("Что сделать дальше?");
            Console.WriteLine($"{(int)Action.SendDirectMessage} - Написать сообщение юзеру");
            Console.WriteLine($"{(int)Action.BroadcastMessage} - Написать широковещательное сообщение");
            Console.WriteLine($"{(int)Action.ExitServer} - Отключиться от сервера");
        }

        static bool RegistrateToServer(string _username)
        {
            if (MessageBuilder.SendRegistrationRequest(_username))
            {
                is_connected_to_server = true;
                Console.WriteLine("Регистрация проведена успешно!");
                return true;
            }
            else
            {
                Console.WriteLine("Не удалось зарегистрироваться!");
                return false;
            }
        }

        static void CheckIncomingMessages(string _username)
        {
            while (true)
            {
                if (is_connected_to_server)
                {
                    Message check_message = MessageBuilder.SendDataRequest(_username);
                    switch (check_message.GetHeader().type)
                    {
                        case MessageType.Empty:
                            {
                                Thread.Sleep(100);
                                break;
                            }
                        default:
                            {
                                Console.WriteLine($"Message from {check_message.GetSender()}: {check_message.GetData()}");
                                break;
                            }
                    }
                }
                else
                    return;
            }
        }

        static void SendDirectMessage(string from_user)
        {
            Console.WriteLine("Кому написать?");
            string recipient = Console.ReadLine();

            Console.WriteLine("Введите текст сообщения");
            string data = Console.ReadLine();

            if (MessageBuilder.SendDirectMessage(from_user, recipient, data))
            {
                Console.WriteLine("Отправлено!");
            }
            else
            {
                Console.WriteLine("Сервер сообщил об ошибке в отправленном сообщении!");
            }
        }

        static void SendBroadcastMessage(string from_user)
        {
            Console.WriteLine("Введите текст сообщения");
            string data = Console.ReadLine();

            if (MessageBuilder.SendBroadcastMessage(from_user, data))
            {
                Console.WriteLine("Отправлено!");
            }
            else
            {
                Console.WriteLine("Сервер сообщил об ошибке в отправленном сообщении!");
            }
        }

        static bool ExitServer(string username)
        {
            if (MessageBuilder.SendExitRequest(username))
            {
                Console.WriteLine("Вы отключились от сервера!");
                is_connected_to_server = false;
                return true;
            }
            else
            {
                Console.WriteLine("Сервер сообщил об ошибке!");
                return false;
            }
        }

        static void Main(string[] args)
        {
            Console.WriteLine("Введите имя для подключения к серверу");
            string username = Console.ReadLine();

            if(RegistrateToServer(username))
            {
                Thread receiving_thread = new Thread(()=>CheckIncomingMessages(username));
                receiving_thread.Start();

                while (true)
                {
                    PrintMenu();
                    int user_action = Convert.ToInt32(Console.ReadLine());
                    switch (user_action)
                    {
                        case (int)Action.SendDirectMessage:
                            {
                                SendDirectMessage(username);
                                break;
                            }
                        case (int)Action.BroadcastMessage:
                            {
                                SendBroadcastMessage(username);
                                break;
                            }
                        case (int)Action.ExitServer:
                            {
                                if (ExitServer(username))
                                {
                                    string s = Console.ReadLine();
                                    return;
                                }
                                else
                                    break;
                            }
                    }
                }
            }
        }
    }
}
