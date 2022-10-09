#pragma once
#include "framework.h"

enum class MessageType
{ Registration, Exit, Peer2Peer, Request, Empty, Confirm};

enum class MessageClient
{ Broker, User, All};

struct MessageHeader
{
	MessageClient recipient;
	int recipient_id_size;
	std::string recipient_id = "";

	MessageClient sender;
	int sender_id_size;
	std::string sender_id = "";

	MessageType type;
	int size;
};

class Message
{
private:
	MessageHeader _header;
	std::string _data;
public:
	Message();
	Message(const MessageHeader& header, const std::string& data);
	Message(MessageHeader&& header, std::string&& data);

	static Message read(CSocket& source);
	static bool send(CSocket& destination, const Message& message);

	std::string getSender() const;

	friend class MessageHandler;
};

