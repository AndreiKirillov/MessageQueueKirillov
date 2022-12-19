#!/usr/bin/env python3
import cgi
import html, threading, sys, os
from Message import *
from cgi_scripts import * 

currentdir = os.path.dirname(os.path.realpath(__file__))
parentdir = os.path.dirname(currentdir)
sys.path.append(parentdir)
import main as global_vars
#from main import is_connected_to_server
#from main import thread_for_incoming_msg


def CheckIncomingMessages(username):
    while True:
        if global_vars.is_connected_to_server:
            check_message = Message()
            answer = Message()
            answer = check_message.SendDataRequest(username)
            if answer._header.type == EMPTY:
                time.sleep(0.1)
            else:
                pass

loginform = cgi.FieldStorage()
username = loginform.getfirst("user_login")  # получаем имя пользователя из формы

if username != None:                   
    username = html.escape(username)     # экранируем, если поле не пустое
    registration_message = Message()
    if registration_message.SendRegistrationRequest(username):    # Отправляем запрос регистрации
        global_vars.is_connected_to_server = True
        global_vars.thread_for_incoming_msg = threading.Thread(target=CheckIncomingMessages, args = (username,))
        global_vars.thread_for_incoming_msg.start()
        print(f"Set-cookie: username={username}")
        afterSuccessfullRegistration()
    else:
        # Если запрос регистрации не удался
        registrationFailed()
# Если пользователь оставил поле пустым
else:
    emptyLoginForm()

