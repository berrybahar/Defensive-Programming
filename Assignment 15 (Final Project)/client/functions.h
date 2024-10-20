#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <filesystem>
#include "serverAnswer.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

unsigned long long littleEndianHexToInt(const std::string& hexStr);
std::string sendFileRequest(tcp::socket& sock, const std::string& req, const std::string& crcReq, const std::string& crcFourthReq, const std::string& reqAgain, const std::string& errMsg, ServerAnswer& serverAnswer, unsigned long& crc);
std::string request(tcp::socket& sock, const std::string& req, const std::string& reqAgain, const std::string& errMsg, ServerAnswer& serverAnswer);
void getTransferInfo(std::string& address, std::string& port, std::string& clientName, std::string& filePathForSending);
void getMeInfo(std::string& clientName, std::string& clientID, std::string& privateKey);
void sendData(tcp::socket& socket, const std::string& data);
std::string readData(tcp::socket& socket);

const std::string base64Chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";
unsigned char hexCharToByte(char hex);
std::string hexToBase64(const std::string& hex);

#endif 