#!/usr/bin/env python3
import cgi, os, sys
import html
from Message import *
from cgi_scripts import * 

#import global_vars
#import global_access

loginform = cgi.FieldStorage()
username = loginform.getfirst("user_login")  # получаем имя пользователя из формы

if username != None:                   
    username = html.escape(username)     # экранируем, если поле не пустое
    registration_message = Message()
    if registration_message.SendRegistrationRequest(username):    # Отправляем запрос регистрации
        #global_access.update_user(global_vars.username)
        file = open("username.txt", "w")
        file.write(username)
        file.close()
        afterSuccessfullRegistration()
    else:
        # Если запрос регистрации не удался
        registrationFailed()
# Если пользователь оставил поле пустым
else:
    emptyLoginForm()

