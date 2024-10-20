#include "serverAnswer.h"

ServerAnswer::ServerAnswer(std::string answerFromServer)
{
	this->version = answerFromServer.substr(0, 2);
	this->code = answerFromServer.substr(2, 4);
	this->payloadSize = answerFromServer.substr(6, 8);
	this->payload = answerFromServer.substr(14);
}

std::string ServerAnswer::toString()
{
	return this->version + this->code + this->payloadSize + this->payload;
}

std::string answer(ServerAnswer serverAnswer)
{
	if (serverAnswer.getCode() == intToHexLittleEndian(SUCCESSFUL_REGISTRATION, 2))
	{
		std::cout << std::endl << "Successfully registered to the server." << std::endl;
	}
	else if (serverAnswer.getCode() == intToHexLittleEndian(RECEIVED_PUBLIC_KEY_SEND_AES_KEY, 2))
	{
		std::cout << std::endl << "Sent the public key and got the AES key." << std::endl;
	}
	else if (serverAnswer.getCode() == intToHexLittleEndian(RECEIVED_FILE_WITH_PROPER_CRC, 2))
	{
		std::cout << std::endl << "The Server got the file with the CRC." << std::endl;
	}
	else if (serverAnswer.getCode() == intToHexLittleEndian(RECEIVE_MESSAGE_CONFIRM, 2))
	{
		std::cout << std::endl << "Server got the message and thanks us." << std::endl;
	}
	else if (serverAnswer.getCode() == intToHexLittleEndian(CONFIRM_RECONNECTION_SEND_AES_KEY, 2))
	{
		std::cout << std::endl << "Successfully recconected to the server and got the AES key." << std::endl;
	}
	return "";
	
	std::cout << std::endl << "server responded with an error" << std::endl;

	return "error";
}