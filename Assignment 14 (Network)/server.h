#ifndef SERVER_H
#define SERVER_H

#include "clientObj.h"
#include <boost/asio.hpp>
#include <cstdlib>
#include <utility>
#include <thread>
#include <Windows.h>

using boost::asio::ip::tcp;
const std::string PORT = "1234";
const int bufferSize = 4096;

void session(tcp::socket sock);
void server(boost::asio::io_context& io_context, unsigned short port);

WCHAR* stringToWCHAR(const std::string& str);
std::string createBackupDirectory(uint64_t userId);
void readSize(tcp::socket& socket, char* buffer, size_t size);
std::string readData(tcp::socket& socket);
void send_data(tcp::socket& socket, const std::string& data);

#endif