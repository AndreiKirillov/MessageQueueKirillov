#include "pch.h"
#include "MessageHandler.h"

bool MessageHandler::isRegistrationMessage(const Message& message)
{
    bool registration_is_correct = (message._header.type == MessageType::Registration);
    bool message_clients_are_correct = (message._header.recipient == MessageClient::Broker && message._header.sender == MessageClient::User);
    
    return registration_is_correct && message_clients_are_correct;
     
}