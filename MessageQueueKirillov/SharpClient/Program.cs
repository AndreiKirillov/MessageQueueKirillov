using System;

namespace SharpClient
{
    class Program
    {
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

        static void Main(string[] args)
        {
            Console.WriteLine("Введите имя для подключения к серверу");
            string username = Console.ReadLine();
            PrintMenu();
        }
    }
}
