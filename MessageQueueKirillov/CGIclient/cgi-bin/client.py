#!/usr/bin/env python3
import cgi, html, http, os, sys
from Message import *
from cgi_scripts import *

currentdir = os.path.dirname(os.path.abspath(__file__))
parentdir = os.path.dirname(currentdir)
sys.path.append(parentdir)
import main as global_vars

# Получаем имя пользователя из куки
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE"))
username = cookie.get("username")

form = cgi.FieldStorage()
recipient = form.getfirst("recipient")
recipient = html.escape(recipient)
message_type = form.getfirst("message_type")
message_type = html.escape(message_type)
data = form.getfirst("data")
data = html.escape(data)

if message_type == "USER":
    message = Message()
    if message.SendDirectMessage(username, recipient, data):
        messageWasSendSuccessfully()
    else:
        messageError()

if message_type == "BROADCAST":
    message = Message()
    if message.SendBroadcastMessage(username, data):
        messageWasSendSuccessfully()
    else:
        messageError()

if message_type == "EXIT":
    message = Message()
    if message.SendExitRequest(username):
        global_vars.is_connected_to_server = False
        exitFromServer()
    else:
        messageError()