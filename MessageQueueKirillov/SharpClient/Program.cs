using System;

namespace SharpClient
{
    class Program
    {
        static void PrintMenu()
        {
            Console.WriteLine("Что сделать дальше?");
            Console.WriteLine("1 - Написать сообщение юзеру");
            Console.WriteLine("2 - Написать широковещательное сообщение");
            Console.WriteLine("3 - Отключиться от сервера");
        }

        static void Main(string[] args)
        {
            PrintMenu();
        }
    }
}
