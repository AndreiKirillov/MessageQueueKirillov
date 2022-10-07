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

	std::vector<char> v(header.size);
	source.Receive(&v[0], header.size);

	return Message(std::move(header), std::string(&v[0], v.size()));
}

bool Message::send(CSocket& destination, const Message& message)
{
	if (destination.Send(&message._header, sizeof(MessageHeader)) == SOCKET_ERROR)
		return false;

	if (destination.Send(message._data.c_str(), message._data.size()) == SOCKET_ERROR)
		return false;
	else
		return true;
}
