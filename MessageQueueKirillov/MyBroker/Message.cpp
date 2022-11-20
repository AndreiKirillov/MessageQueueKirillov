#include "pch.h"
#include "Message.h"

//Message::Message(): _header(), _data("")
//{
//}
//
//Message::Message(const MessageHeader& header, const std::string& data): _header(header), _data(data)
//{
//}

//Message::Message(MessageHeader&& header, std::string&& data): _header(std::move(header)), _data(std::move(data))
//{
//}

//Message Message::read(CSocket& source)
//{
//	MessageHeader header;
//	source.Receive(&header, sizeof(MessageHeader));
//
//	if (header.recipient == MessageClient::User)
//	{
//		std::vector<char> recipient(header.recipient_id_size);
//		source.Receive(&recipient[0], header.recipient_id_size);
//		header.recipient_id = std::string(&recipient[0], header.recipient_id_size);
//	}
//
//	if (header.sender == MessageClient::User)
//	{
//		std::vector<char> sender(header.sender_id_size);
//		source.Receive(&sender[0], header.sender_id_size);
//		header.sender_id = std::string(&sender[0], header.sender_id_size);
//	}
//
//	if (header.size > 0)
//	{
//		std::vector<char> message(header.size);
//		source.Receive(&message[0], header.size);
//		return Message(std::move(header), std::string(&message[0], message.size()));
//	}
//
//	return Message(std::move(header), std::string());
//}

//bool Message::send(CSocket& destination, const Message& message)
//{
//	// отправляем заголовок
//	if (destination.Send(&message._header, sizeof(MessageHeader)) == SOCKET_ERROR)
//		return false;
//
//	if (message._header.recipient == MessageClient::User)
//	{
//		// отправляем имя отправителя
//		if (destination.Send(message._header.recipient_id.c_str(), message._header.recipient_id_size) == SOCKET_ERROR)
//			return false;
//	}
//	if (message._header.sender == MessageClient::User)
//	{
//		// отправляем имя получателя
//		if (destination.Send(message._header.sender_id.c_str(), message._header.sender_id_size) == SOCKET_ERROR)
//			return false;
//	}
//
//	// отправляем само сообщение
//	if (destination.Send(message._data.c_str(), message._data.size()) == SOCKET_ERROR)
//		return false;
//	else
//		return true;
//}

// Функция отправки запроса регистрации
//bool Message::sendRegistrationRequest(const std::string& username)
//{
//	MessageHeader header;
//	header.sender = MessageClient::User;
//	header.sender_id = username;
//	header.sender_id_size = username.size();
//
//	header.recipient = MessageClient::Broker;
//
//	header.type = MessageType::Registration;
//	header.size = 0;
//
//	CSocket server_sock;
//	server_sock.Create();
//	if (!server_sock.Connect("127.0.0.1", 12345))
//	{
//		return false;
//	}
//
//	send(server_sock, Message(header)); // Отправляем запрос регистрации
//
//	return waitConfirm(server_sock); // получаем подтверждение регистрации или сообщение об ошибке
//}

void Message::sendConfirm(CSocket& destination)   // функция отправки подтверждения 
{
	MessageHeader confirm;
	confirm.type = MessageType::Confirm;
	confirm.sender = MessageClient::Broker;
	confirm.recipient = MessageClient::User;
	confirm.sender_id_size = 0;
	confirm.size = 0;
	destination.Send(&confirm, sizeof(MessageHeader));
}

void Message::sendError(CSocket& destination)     // функция отправки сообщения об ошибке 
{
	MessageHeader error;
	error.type = MessageType::Error;
	error.sender = MessageClient::Broker;
	error.recipient = MessageClient::User;
	error.sender_id_size = 0;
	error.recipient_id_size = 0;
	error.size = 0;
	destination.Send(&error, sizeof(MessageHeader));
}

void Message::sendEmpty(CSocket& destination)
{
	MessageHeader header;
	header.type = MessageType::Empty;
	header.sender = MessageClient::Broker;
	header.recipient = MessageClient::User;
	header.sender_id_size = 0;
	header.recipient_id_size = 0;
	header.size = 0;
	Message empty_msg(header);
	send(destination, empty_msg);
}

//bool Message::waitConfirm(CSocket& confirm_source)
//{
//	Message confirm_message = read(confirm_source);
//	
//	if (confirm_message.isConfirm())
//		return true;
//	if (confirm_message.isError())
//		return false;
//}

//bool Message::isConfirm()
//{
//	return _header.type == MessageType::Confirm;
//}
//
//bool Message::isError()
//{
//	return _header.type == MessageType::Error;
//}

//MessageHeader Message::getHeader() const
//{
//	return _header;
//}

//std::string Message::getSender() const
//{
//	return _sender_id;
//}

//std::string Message::getRecipient() const
//{
//	return _recipient_id;
//}

bool Message::isRegistrationMessage() const
{
	bool registration_is_correct = (_header.type == MessageType::Registration);
	bool message_clients_are_correct = (_header.recipient == MessageClient::Broker && _header.sender == MessageClient::User);

	return registration_is_correct && message_clients_are_correct;
}
