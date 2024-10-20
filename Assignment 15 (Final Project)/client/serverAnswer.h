#ifndef SERVER_ANSWER_H
#define SERVER_ANSWER_H

#include "clientRequest.h"

#define SUCCESSFUL_REGISTRATION 1600
#define FAILED_REGISTRATION 1601
#define RECEIVED_PUBLIC_KEY_SEND_AES_KEY 1602
#define RECEIVED_FILE_WITH_PROPER_CRC 1603
#define RECEIVE_MESSAGE_CONFIRM 1604
#define CONFIRM_RECONNECTION_SEND_AES_KEY 1605
#define FAILED_RECONNECTION 1606
#define FAILED_REQUEST  1607

class ServerAnswer
{
		std::string version;
		std::string code;
		std::string payloadSize;
		std::string payload;
	public:
		ServerAnswer(std::string answerFromServer);
		ServerAnswer() {}
		std::string getCode() { return this->code; }
		std::string getPayload() { return this->payload; }
		std::string toString();
};

std::string answer(ServerAnswer serverAnswer);

#endif