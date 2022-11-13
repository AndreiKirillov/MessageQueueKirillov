#include "pch.h"
#include "Message.h"

Message::Message(): _header(), _data("")
{
}

Message::Message(const MessageHeader& header, const std::string& data): _header(header), _data(data)
{
}

Message::Message(MessageHeader&& header, std::string&& data): _header(std::move(header)), _data(std::move(data))
{
}

Message Message::read(CSocket& source)
{
	MessageHeader header;
	source.Receive(&header, sizeof(MessageHeader));

	if (header.recipient == MessageClient::User)
	{
		std::vector<char> recipient(header.recipient_id_size);
		source.Receive(&recipient[0], header.recipient_id_size);
		header.recipient_id = std::string(&recipient[0], header.recipient_id_size);
	}

	if (header.sender == MessageClient::User)
	{
		std::vector<char> sender(header.sender_id_size);
		source.Receive(&sender[0], header.sender_id_size);
		header.sender_id = std::string(&sender[0], header.sender_id_size);
	}

	if (header.size > 0)
	{
		std::vector<char> message(header.size);
		source.Receive(&message[0], header.size);
		return Message(std::move(header), std::string(&message[0], message.size()));
	}

	return Message(std::move(header), std::string());
}

bool Message::send(CSocket& destination, const Message& message)
{
	// отправляем заголовок
	if (destination.Send(&message._header, sizeof(MessageHeader)) == SOCKET_ERROR)
		return false;

	if (message._header.recipient == MessageClient::User)
	{
		// отправляем имя отправителя
		if (destination.Send(message._header.recipient_id.c_str(), message._header.recipient_id_size) == SOCKET_ERROR)
			return false;
	}
	if (message._header.sender == MessageClient::User)
	{
		// отправляем имя получателя
		if (destination.Send(message._header.sender_id.c_str(), message._header.sender_id_size) == SOCKET_ERROR)
			return false;
	}

	// отправляем само сообщение
	if (destination.Send(message._data.c_str(), message._data.size()) == SOCKET_ERROR)
		return false;
	else
		return true;
}

void Message::sendConfirm(CSocket& destination)   // функция отправки подтверждения 
{
	MessageHeader confirm;
	confirm.type = MessageType::Confirm;
	destination.Send(&confirm, sizeof(MessageHeader));
}

void Message::sendError(CSocket& destination)     // функция отправки сообщения об ошибке 
{
	MessageHeader error;
	error.type = MessageType::Error;
	destination.Send(&error, sizeof(MessageHeader));
}

MessageHeader Message::getHeader() const
{
	return _header;
}

std::string Message::getSender() const
{
	return _header.sender_id;
}

std::string Message::getRecipient() const
{
	return _header.recipient_id;
}

bool Message::isRegistrationMessage() const
{
	bool registration_is_correct = (_header.type == MessageType::Registration);
	bool message_clients_are_correct = (_header.recipient == MessageClient::Broker && _header.sender == MessageClient::User);

	return registration_is_correct && message_clients_are_correct;
}
