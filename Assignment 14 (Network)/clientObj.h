#ifndef CLIENT_OBJ_H
#define CLIENT_OBJ_H

#include <iostream> 
#include <vector> 
#include <string>
#include <filesystem>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>

#define SERVER_VERSION 1

#define SAVE_FILE 100
#define RETRIEVE_FILE  200
#define DELETE_FILE  201
#define GET_CLIENT_FILE_LIST  202

#define FILE_RETRIEVED 210
#define RETURN_CLIENT_FILE_LIST 211
#define FILE_SAVED_OR_DELETED 212
#define FILE_NOT_EXIST 1001
#define CLIENT_NOT_HAVE_FILE 1002
#define SERVER_PROBLEM 1003

class Client
{
	std::string _userId;
	std::string _version;
	std::string _op;
	std::string _nameLen;
	std::string _filename;
	std::string _size;
	std::string _payload;
public:
	Client(std::string clientRequest);
	~Client();
	std::string getUserId() { return _userId; }
	std::string getVersion() { return _version; }
	std::string getOp() { return _op; }
	std::string getNameLen() { return _nameLen; }
	std::string getFileName() { return _filename; }
	std::string getSize() { return _size; }
	std::string getPayLoad() { return _payload; }
};

std::vector<unsigned char> hexStringToBytes(const std::string& hexStr);
std::string deleteNullString(const std::string& str);
std::string byteStringToHexString(const std::string& byteStr);
std::string stringToHex(const std::string& input);
std::string intToHex(int value, size_t byteSize);
std::string generateRandomString(size_t length);
std::vector<std::string> listFiles(const std::string& path);
std::string hexStringToString(const std::string& hex);
uint64_t hexStringToNumber(const std::string& hexString);
std::string saveFile(Client client, std::string backupFileDirectory);
std::string retrieveFile(Client client, std::string backupFileDirectory, std::string clientFilePath);
std::string deleteFile(Client client, std::string backupFileDirectory);
std::string getClientFileList(Client client, std::string backupFileDirectory, std::string clientFilePath);
void answer(Client client, std::string& answerToClient, std::string backupFileDirectory, std::string clientFilePath);

#endif