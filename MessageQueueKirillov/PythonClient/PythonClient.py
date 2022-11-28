import threading
from dataclasses import dataclass
import socket, struct, time
from Message import *

is_connected_to_server:bool = True

def PrintMenu():
    print("Что сделать дальше?")
    print("1 - Написать сообщение юзеру")
    print("2 - Написать широковещательное сообщение")
    print("3 - Отключиться от сервера")

def CheckIncomingMessages(username):
    while True:
        if is_connected_to_server:
            check_message = Message()
            answer = Message()
            answer = check_message.SendDataRequest(username)
            if answer._header.type == EMPTY:
                time.sleep(0.1)
            else:
                print(f"Message from {answer._sender_id}: {answer._data}")

def Main():
    print("Введите имя для подключения к серверу");
    username = input();

    registration_message = Message()
    if registration_message.SendRegistrationRequest(username):
        is_connected_to_server = True
        t = threading.Thread(target=CheckIncomingMessages, args = (username,))
        t.start()

        while True:
            PrintMenu()
            user_action = input()
            user_action = int(user_action)
            if user_action == 1:
                print("Кому написать?")
                recipient = input()
                print("Введите текст сообщения")
                data = input()

                message = Message()
                if message.SendDirectMessage(username, recipient, data):
                    print("Отправлено!")
                else:
                    print("Сервер сообщил об ошибке в отправленном сообщении!")

            if user_action == 2:
                print("Введите текст сообщения")
                data = input()

                message = Message()
                if message.SendBroadcastMessage(username, data):
                    print("Отправлено!")
                else:
                    print("Сервер сообщил об ошибке в отправленном сообщении!")

            if user_action == 3:
                message = Message()
                if message.SendExitRequest(username):
                    is_connected_to_server = False
                    print("Вы отключились от сервера!")
                    s = input()
                    return
                else:
                    print("Сервер сообщил об ошибке!")

    else:
        print("Не удалось зарегистрироваться!")


Main()
