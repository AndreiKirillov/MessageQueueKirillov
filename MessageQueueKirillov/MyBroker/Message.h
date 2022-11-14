#pragma once
#include "framework.h"

enum class MessageType
{ Registration, Exit, Peer2Peer, GetData, Empty, Confirm, Error};

enum class MessageClient
{ Broker, User, All};

struct MessageHeader
{
	MessageClient recipient;
	int recipient_id_size;
	//std::string recipient_id = "";

	MessageClient sender;
	int sender_id_size;
	//std::string sender_id = "";

	MessageType type;
	int size;
};

class Message
{
private:
	MessageHeader _header;
	std::string _recipient_id;
	std::string _sender_id;
	std::string _data;
public:
	Message() :_header(), _recipient_id(""), _sender_id(""), _data("")
	{}
	Message(const MessageHeader& header, const std::string& data = "") : _header(header), _recipient_id(""), _sender_id(""), _data(data)
	{}
	//Message(MessageHeader&& header, std::string&& data);

	static Message read(CSocket& source)
	{
		Message received_message;
		MessageHeader header;
		source.Receive(&header, sizeof(MessageHeader));

		if (header.type == MessageType::Confirm || header.type == MessageType::Error)
		{
			received_message._header = header;
			return received_message;
		}

		if (header.recipient == MessageClient::User)
		{
			std::vector<char> recipient(header.recipient_id_size);
			source.Receive(&recipient[0], header.recipient_id_size);
			received_message._recipient_id = std::string(&recipient[0], header.recipient_id_size);
		}

		if (header.sender == MessageClient::User)
		{
			std::vector<char> sender(header.sender_id_size);
			source.Receive(&sender[0], header.sender_id_size);
			received_message._sender_id = std::string(&sender[0], header.sender_id_size);
		}

		received_message._header = header;
		if (header.size > 0)
		{
			std::vector<char> data(header.size);
			source.Receive(&data[0], header.size);
			received_message._data = std::string(&data[0], data.size());
		}

		return received_message;
	}
	static bool send(CSocket& destination, Message& message)
	{

		// отправляем заголовок
		if (destination.Send(&message._header, sizeof(MessageHeader)) == SOCKET_ERROR)
			return false;

		if (message._header.recipient == MessageClient::User)
		{
			// отправляем имя отправителя
			if (destination.Send(message._recipient_id.c_str(), message._header.recipient_id_size) == SOCKET_ERROR)
				return false;
		}
		if (message._header.sender == MessageClient::User)
		{
			// отправляем имя получателя
			if (destination.Send(message._sender_id.c_str(), message._header.sender_id_size) == SOCKET_ERROR)
				return false;
		}

		// отправляем само сообщение
		if (message._data.empty())
			return true;
		else
		{
			if (destination.Send(message._data.c_str(), message._data.size()) == SOCKET_ERROR)
				return false;
			else
				return true;
		}
	}
	static bool sendRegistrationRequest(const std::string& username)
	{
		MessageHeader header;
		header.sender = MessageClient::User;
		header.sender_id_size = username.size();

		header.recipient = MessageClient::Broker;
		header.recipient_id_size = 0;

		header.type = MessageType::Registration;
		header.size = 0;
		Message m(header);
		m._sender_id = username;

		CSocket server_sock;
		server_sock.Create();
		if (!server_sock.Connect("127.0.0.1", 12345))
		{
			return false;
		}
		std::cout << "socket.connect" << std::endl;
		send(server_sock, m); // Отправляем запрос регистрации
		std::cout << "send отработал!" << std::endl;
		return waitConfirm(server_sock); // получаем подтверждение регистрации или сообщение об ошибке
	}

	static void sendConfirm(CSocket& destination);
	static void sendError(CSocket& destination);
	static bool waitConfirm(CSocket& confirm_source)
	{
		Message confirm_message = read(confirm_source);

		if (confirm_message.isConfirm())
			return true;
		if (confirm_message.isError())
			return false;
	}
	bool isConfirm()
	{
		return _header.type == MessageType::Confirm;
	}
	bool isError()
	{
		return _header.type == MessageType::Error;
	}


	MessageHeader getHeader() const;
	std::string getSender() const;
	std::string getRecipient() const;

	bool isRegistrationMessage() const;

};

