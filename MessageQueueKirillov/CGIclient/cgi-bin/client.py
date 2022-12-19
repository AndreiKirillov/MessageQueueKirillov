#!/usr/bin/env python3
import cgi, html, os, sys
from Message import *
from cgi_scripts import *

#import global_vars
#import global_access

form = cgi.FieldStorage()
recipient = form.getfirst("recipient")
recipient = html.escape(recipient)
message_type = form.getfirst("message_type")
message_type = html.escape(message_type)
data = form.getfirst("data")
data = html.escape(data)

file = open("username.txt", "r")
username = file.read()
file.close()

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
        exitFromServer()
    else:
        messageError()