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

	static void sendConfirm(CSocket& destination);
	static void sendError(CSocket& destination);


	MessageHeader getHeader() const;
	std::string getSender() const;
	std::string getRecipient() const;

	bool isRegistrationMessage() const;

};

