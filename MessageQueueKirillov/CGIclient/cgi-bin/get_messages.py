
from Message import *
from cgi_scripts import *

file = open("username.txt", "r")
username = file.read()
file.close()

m = Message()
request = m.SendDataRequest(username)

incoming_messages = request._data.split('\n')
showAllMessages(incoming_messages)
