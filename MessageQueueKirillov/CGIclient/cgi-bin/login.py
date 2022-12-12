#!/usr/bin/env python3
import cgi
import html
from Message import *

loginform = cgi.FieldStorage()
username = loginform.getfirst("user_login")  # получаем имя пользователя из формы

if username != None:                   
    username = html.escape(username)     # экранируем, если поле не пустое
    registration_message = Message()
    if registration_message.SendRegistrationRequest(username):    # Отправляем запрос регистрации
        print("Content-type: text/html; charset=utf-8\n\n")
        print("""<!DOCTYPE HTML>
                <html>
                <head>
                    <meta charset="utf-8">
                    <title>Log in</title>
                </head>
                <body>
                    <h1>You logged in successfully!</h1>
                    <form action="../main_page.html">
                    <button>Begin work</button>
                    </form>
                </body>
                </html>""")
    else:
        # Если запрос регистрации не удался
        print("Content-type: text/html; charset=utf-8\n\n")
        print("""<!DOCTYPE HTML>
                <html>
                <head>
                    <meta charset="utf-8">
                    <title>Login error</title>
                </head>
                <body>""")
        print("<h1>You cannot log in the server!</h1>")
        print("""</body>
            </html>""")
# Если пользователь оставил поле пустым
else:
    print("Content-type: text/html; charset=utf-8\n\n")
    print("""<!DOCTYPE HTML>
            <html>
            <head>
                <meta charset="utf-8">
                <title>Login warning</title>
            </head>
            <body>""")
    print("<h1>You didnt input your name!</h1>")
    print("""</body>
        </html>""")

