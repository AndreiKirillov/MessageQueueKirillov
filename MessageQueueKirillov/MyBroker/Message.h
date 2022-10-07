#pragma once
#include "framework.h"

enum class MessageType
{ Init, Exit, Peer2Peer, Request, Empty, Confirm};

enum class Recipient
{ Broker, User, All};

struct MessageHeader
{
	Recipient recipient;
	std::string sender_id;
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
};

