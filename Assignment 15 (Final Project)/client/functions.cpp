#include "functions.h"

unsigned long long littleEndianHexToInt(const std::string& hexStr)
{

	unsigned long long result = 0;

	for (std::size_t i = 0; i < hexStr.length(); i += 2) 
	{
		std::string byteStr = hexStr.substr(i, 2);  
		unsigned int byteVal;
		std::stringstream ss;
		ss << std::hex << byteStr;  
		ss >> byteVal;

		result |= static_cast<unsigned long long>(byteVal) << (i * 4);
	}

	return result;
}

std::string sendFileRequest(tcp::socket& sock, const std::string& req, const std::string& crcReq, const std::string& crcFourthReq, const std::string& reqAgain, const std::string& errMsg, ServerAnswer& serverAnswer, unsigned long& crc)
{ 
	std::string ans;
	ServerAnswer serverAns;
	for (int i = 1; i <= 4; i++)
	{
		if (i >= 2)
		{
			// CRC is not proper
			sendData(sock, crcReq);
			serverAns = ServerAnswer(readData(sock));
			ans = answer(serverAns);
		}

		// sending the file
		sendData(sock, req);
		serverAns = ServerAnswer(readData(sock));
		ans = answer(serverAns);

		unsigned long crcFromServer = littleEndianHexToInt((serverAns.getPayload()).substr(550));

		if (crc == crcFromServer)
		{
			std::cout << std::endl << "CRC is alright! The file is successfully saved..." << std::endl;
			break;
		}
		else
		{
			if (i == 4)
			{
				sendData(sock, crcFourthReq);
				serverAns = ServerAnswer(readData(sock));
				ans = answer(serverAns);

				std::cerr << std::endl << "fatal error: " << errMsg << std::endl;
				return "fatal";
			}
			else
			{
				std::cout << reqAgain << std::endl;
			}
		}
	}
	return "";
}

std::string request(tcp::socket& sock, const std::string& req, const std::string& reqAgain, const std::string& errMsg, ServerAnswer& serverAnswer)
{
	std::string ans;
	for (int i = 1; i <= 4; i++)
	{
		sendData(sock, req);
		serverAnswer = ServerAnswer(readData(sock));
		ans = answer(serverAnswer);

		if (serverAnswer.getCode() == intToHexLittleEndian(FAILED_RECONNECTION, 4))
		{
			return "recconection failed";
		}
		else if (ans != "error")
		{
			break;
		}
		else
		{
			if (i == 4)
			{
				std::cerr << std::endl << "fatal error: " << errMsg << std::endl;
				return "fatal";
			}
			else
			{
				std::cout << reqAgain << std::endl;
			}
		}
	}
	return "";
}

void getMeInfo(std::string& clientName, std::string& clientID, std::string& privateKey)
{
	std::ifstream meInfoFile("me.info");

	if (!meInfoFile)
		return;

	int i = 1;
	std::string line;
	while (std::getline(meInfoFile, line))
	{
		if (i == 1)
			clientName = line;
		else if (i == 2)
			clientID = line;
		else if (i == 3 && !std::filesystem::exists("priv.key"))
			privateKey = line;
		i++;
	}

	meInfoFile.close();

	if (privateKey != "")
	{
		std::ofstream privKeyFile("priv.key");
		
		if (privKeyFile.is_open())
		{
			privKeyFile << privateKey;
		}
		else
		{
			std::cerr << std::endl << "Unable to create priv.key file!" << std::endl;
			return;
		}
	}

	std::ifstream privKeyFile("priv.key");

	if (!privKeyFile)
		return;

	std::getline(privKeyFile, line);
	privateKey = line;
	privKeyFile.close();
}

void getTransferInfo(std::string& address, std::string& port, std::string& clientName, std::string& filePathForSending)
{
	std::ifstream transferInfoFile("transfer.info");

	if (!transferInfoFile)
		return;

	int i = 1;
	std::string line;
	while (std::getline(transferInfoFile, line))
	{
		if (i == 1)
		{
			int indexOfColon = line.find(":");
			address = line.substr(0, indexOfColon);
			port = line.substr(indexOfColon + 1, line.size() - indexOfColon);
		}
		else if (i == 2 && clientName == "")
		{
			if (line.size() > 100)
			{
				std::cerr << std::endl << "Client name is longer than 100 characters!" << std::endl;
				return;
			}
			clientName = line;
		}
		else if (i == 3)
		{
			filePathForSending = line;
		}
		i++;
	}

	transferInfoFile.close();
}

void sendData(tcp::socket& socket, const std::string& data) 
{
	// Convert data size to a 4-byte unsigned integer (network byte order)
	uint32_t data_size = static_cast<uint32_t>(data.size());
	uint32_t size_network_order = htonl(data_size);

	// Send the size of the data first
	boost::asio::write(socket, boost::asio::buffer(&size_network_order, sizeof(size_network_order)));

	// Send the actual data
	boost::asio::write(socket, boost::asio::buffer(data));
}

std::string readData(tcp::socket& socket)
{
	char header[4];

	try 
	{
		// Read the size of the data
		boost::asio::read(socket, boost::asio::buffer(header, sizeof(header)));

		// Convert header to length
		uint32_t data_length;
		std::memcpy(&data_length, header, sizeof(data_length));
		data_length = ntohl(data_length);

		std::vector<char> data(data_length);

		// Read the actual data
		boost::asio::read(socket, boost::asio::buffer(data.data(), data_length));

		return std::string(data.begin(), data.end());
	}
	catch (const boost::system::system_error& e) {
		std::cerr << std::endl << "Error reading data: " << e.what() << std::endl;
		return ""; // Return empty string on error
	}
}

unsigned char hexCharToByte(char hex)
{
	if (hex >= '0' && hex <= '9') return hex - '0';
	if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
	if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
	throw std::invalid_argument("Invalid hex character");
}

std::string hexToBase64(const std::string& hex) 
{
	if (hex.length() % 2 != 0) {
		throw std::invalid_argument("Invalid hex string length");
	}

	std::string base64;
	int val = 0;
	int valb = -6;

	// Loop through the hex string, convert to bytes and then to base64
	for (size_t i = 0; i < hex.length(); i += 2)
	{
		unsigned char byte = (hexCharToByte(hex[i]) << 4) | hexCharToByte(hex[i + 1]);
		val = (val << 8) + byte;
		valb += 8;
		while (valb >= 0) {
			base64.push_back(base64Chars[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}

	if (valb > -6) base64.push_back(base64Chars[((val << 8) >> (valb + 8)) & 0x3F]);

	while (base64.size() % 4) base64.push_back('=');

	return base64;
}