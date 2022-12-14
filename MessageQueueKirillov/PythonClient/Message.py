import threading
from dataclasses import dataclass
import socket, struct, time

# enum MessageType
REGISTRATION = 0
EXIT		= 1
PEER2PEER	= 2
BROADCAST	= 3
GET_DATA	= 4
EMPTY	    = 5
CONFIRM     = 6
ERROR       = 7

# enum MessageClient
BROKER	= 0
USER    = 1
ALL		= 2

@dataclass
class MessageHeader:
	recipient_type: int = 0
	recipient_id_size: int = 0
	sender_type: int = 0
	sender_id_size: int = 0
	type: int = 0
	size: int = 0

	def Send(self, s):
		s.send(struct.pack(f'iiiiii', self.recipient_type, self.recipient_id_size, self.sender_type, self.sender_id_size,
					 self.type, self.size))

	def Receive(self, s):
		try:
			(self.recipient_type, self.recipient_id_size, self.sender_type, self.sender_id_size, self.type, self.size) = struct.unpack('iiiiii', s.recv(24))
		except:
			self.size = 0
			self.type = EMPTY


class Message:

	#_header : MessageHeader

	def __init__(self, header = MessageHeader(), data=""):
		self._header = header
		self._recipient_id = ""
		self._sender_id = ""
		self._data = data

	# Функция отправки
	def Send(self, s):
		self._header.Send(s)

		if self._header.type == EMPTY:
			return

		if self._header.recipient_type == USER and self._header.recipient_id_size > 0:
			s.send(struct.pack(f'{self._header.recipient_id_size}s', self._recipient_id.encode('cp866')))

		if self._header.sender_type == USER and self._header.sender_id_size > 0:
			s.send(struct.pack(f'{self._header.sender_id_size}s', self._sender_id.encode('cp866')))

		if self._header.size > 0:
			s.send(struct.pack(f'{self._header.size}s', self._data.encode('cp866')))


	# Функция чтения
	def Receive(self, s):
		self._header.Receive(s)

		if self._header.type == EMPTY or self._header.type == CONFIRM or self._header.type == ERROR:
			return

		if self._header.recipient_id_size > 0:
			self._recipient_id = struct.unpack(f'{self._header.recipient_id_size}s', s.recv(self._header.recipient_id_size))[0].decode('cp866')

		if self._header.sender_id_size > 0:
			self._sender_id = struct.unpack(f'{self._header.sender_id_size}s', s.recv(self._header.sender_id_size))[0].decode('cp866')

		if self._header.size > 0:
			self._data = struct.unpack(f'{self._header.size}s', s.recv(self._header.size))[0].decode('cp866')

	@staticmethod
	def WaitConfirm(s):
		m = Message()
		m.Receive(s)
		if m._header.type == CONFIRM:
			return True
		if m._header.type == ERROR:
			return False


	def SendRegistrationRequest(self,username):
		HOST = 'localhost'
		PORT = 12345
		with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
			s.connect((HOST, PORT))
			header = MessageHeader()
			header.recipient_type = BROKER
			header.recipient_id_size = 0
			header.sender_type = USER
			header.sender_id_size = len(username)
			header.type = REGISTRATION
			header.size = 0
			message = Message(header)
			message._sender_id = username
			
			message.Send(s)
			res = Message.WaitConfirm(s)
			return res

	def SendDataRequest(self, username):
		HOST = 'localhost'
		PORT = 12345
		with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
			s.connect((HOST, PORT))
			header = MessageHeader()
			header.recipient_type = BROKER
			header.recipient_id_size = 0
			header.sender_type = USER
			header.sender_id_size = len(username)
			header.type = GET_DATA
			header.size = 0
			message = Message(header)
			message._sender_id = username
			
			message.Send(s)
			res = Message()
			res.Receive(s)
			return res



	def SendDirectMessage(self, sender, recipient, data):
		HOST = 'localhost'
		PORT = 12345
		with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
			s.connect((HOST, PORT))
			header = MessageHeader()
			header.recipient_type = USER
			header.recipient_id_size = len(recipient)
			header.sender_type = USER
			header.sender_id_size = len(sender)
			header.type = PEER2PEER
			header.size = len(data)
			message = Message(header)
			message._sender_id = sender
			message._recipient_id = recipient
			message._data = data
			
			message.Send(s)
			res = Message.WaitConfirm(s)
			return res

	def SendBroadcastMessage(self, sender, data):
		HOST = 'localhost'
		PORT = 12345
		with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
			s.connect((HOST, PORT))
			header = MessageHeader()
			header.recipient_type = ALL
			header.recipient_id_size = 0
			header.sender_type = USER
			header.sender_id_size = len(sender)
			header.type = BROADCAST
			header.size = len(data)
			message = Message(header)
			message._sender_id = sender
			message._data = data
			
			message.Send(s)
			res = Message.WaitConfirm(s)
			return res

	def SendExitRequest(self, sender):
		HOST = 'localhost'
		PORT = 12345
		with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
			s.connect((HOST, PORT))
			header = MessageHeader()
			header.recipient_type = BROKER
			header.recipient_id_size = 0
			header.sender_type = USER
			header.sender_id_size = len(sender)
			header.type = EXIT
			header.size = 0
			message = Message(header)
			message._sender_id = sender
			
			message.Send(s)
			res = Message.WaitConfirm(s)
			return res