#!/usr/bin/env python3
import cgi
import html
from Message import *
from Actions import *

loginform = cgi.FieldStorage()
username = loginform.getfirst("user_login")  # получаем имя пользователя из формы

if username != None:                   
    username = html.escape(username)     # экранируем, если поле не пустое
    registration_message = Message()
    if registration_message.SendRegistrationRequest(username):    # Отправляем запрос регистрации
        print(f"Set-cookie: username={username}")
        afterSuccessfullRegistration()
    else:
        # Если запрос регистрации не удался
        registrationFailed()
# Если пользователь оставил поле пустым
else:
    emptyLoginForm()

