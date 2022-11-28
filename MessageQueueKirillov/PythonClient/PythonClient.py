import threading
from dataclasses import dataclass
import socket, struct, time
from Message import *

def PrintMenu():
    print("Что сделать дальше?")
    print("1 - Написать сообщение юзеру")
    print("2 - Написать широковещательное сообщение")
    print("3 - Отключиться от сервера")

def Main():
    PrintMenu()

Main()
