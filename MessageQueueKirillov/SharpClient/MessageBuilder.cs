using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace SharpClient
{
    class MessageBuilder
    {
        public static bool SendRegistrationRequest(string username)
        {
			MessageHeader header = new MessageHeader();

			header.recipient = MessageClient.Broker;
			header.recipient_id_size = 0;

			header.type = MessageType.Registration;
			header.size = 0;
			Message request = new Message(ref header);
			request.SetSender(username);

			int nPort = 12345;
			IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);
			Socket server_sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			server_sock.Connect(endPoint);
			if (!server_sock.Connected)
			{
				throw new Exception("Connection error");
			}

			if (Message.Send(server_sock, request)) // Отправляем запрос регистрации
				return Message.WaitConfirm(server_sock); // получаем подтверждение регистрации или сообщение об ошибке
			else
				return false;
		}

		public static Message sendDataRequest(string username)   // Функция отправки запроса данных
		{
			MessageHeader header = new MessageHeader();

			header.recipient = MessageClient.Broker;
			header.recipient_id_size = 0;

			header.type = MessageType.GetData;
			header.size = 0;
			Message request = new Message(ref header);
			request.SetSender(username);

			int nPort = 12345;
			IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);
			Socket server_sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			server_sock.Connect(endPoint);
			if (!server_sock.Connected)
			{
				Message m = new Message();
				m.SetType(MessageType.Empty);
				return m;
			}


			if (Message.Send(server_sock, request)) // Отправляем запрос регистрации
				return Message.Read(server_sock); // получаем подтверждение регистрации или сообщение об ошибке
			else
            {
				Message m = new Message();
				m.SetType(MessageType.Empty);
				return m;
			}
		}

		public static bool sendDirectMessage(string sender, string recipient, string data)
		{
			MessageHeader header = new MessageHeader();
			header.type = MessageType.Peer2Peer;
			Message message = new Message(ref header);
			message.SetSender(sender);
			message.SetRecipient(recipient);
			message.SetData(data);

			int nPort = 12345;
			IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);
			Socket server_sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			server_sock.Connect(endPoint);
			if (!server_sock.Connected)
			{
				Message.Send(server_sock, message);
				if (Message.WaitConfirm(server_sock))
					return true;
				else
					return false;
			}
			else
				return false;
		}

		public static bool sendBroadcastMessage(string sender, string data)
		{
			MessageHeader header = new MessageHeader();
			header.type = MessageType.Broadcast;
			header.recipient = MessageClient.All;
			Message message = new Message(ref header);
			message.SetSender(sender);
			message.SetData(data);

			int nPort = 12345;
			IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);
			Socket server_sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			server_sock.Connect(endPoint);
			if (!server_sock.Connected)
			{
				Message.Send(server_sock, message);
				if (Message.WaitConfirm(server_sock))
					return true;
				else
					return false;
			}
			else
				return false;
		}

	}
}
