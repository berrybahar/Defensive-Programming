#include "clientRequest.h"

std::string intToHexLittleEndian(int value, int byteCount) 
{
	std::ostringstream hexStream;

	// Ensure the output is in little endian order
	for (int i = 0; i < byteCount; ++i) {
		int byte = (value >> (i * 8)) & 0xFF;
		hexStream << std::setfill('0') << std::setw(2) << std::hex << byte;
	}

	return hexStream.str();
}

std::string toHexString(const std::string& input, std::size_t byteCount) 
{
	std::ostringstream hexStream;
	std::size_t i;

	// Convert each character of the input string to hex
	for (i = 0; i < input.size() && i < byteCount; ++i)
		hexStream << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(input[i]) & 0xFF);

	// If the input string is shorter than the byteCount, pad with zeros
	while (i < byteCount) 
	{
		hexStream << "00";
		++i;
	}

	return hexStream.str();
}

unsigned long memcrc(char* b, size_t n)
{
	unsigned int v = 0, c = 0;
	unsigned long s = 0;
	unsigned int tabidx;

	for (int i = 0; i < n; i++)
	{
		tabidx = (s >> 24) ^ (unsigned char)b[i];
		s = UNSIGNED((s << 8)) ^ crctab[0][tabidx];
	}

	while (n)
	{
		c = n & 0377;
		n = n >> 8;
		s = UNSIGNED(s << 8) ^ crctab[0][(s >> 24) ^ c];
	}
	return (unsigned long)UNSIGNED(~s);
}

std::string byteToHex(const std::string& input)
{
	std::ostringstream oss;

	for (unsigned char byte : input)
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);

	return oss.str();
}

std::string readFile(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::binary);

	if (!file.is_open())
		std::cerr << "Error: Could not open the file." << std::endl;

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::string fileContent(fileSize, '\0');

	file.read(&fileContent[0], fileSize);

	file.close();

	return fileContent;
}

std::string getSendFilePayload(const std::string& fileName, int packetNum, int totalPackets, AESWrapper& aes, unsigned long& crc)
{
	std::string payload;
	std::string hexPacketNum = intToHexLittleEndian(packetNum, 2);
	std::string hexTotalPackets = intToHexLittleEndian(totalPackets, 2);

	std::string fileData = readFile(fileName);
	crc = memcrc(const_cast<char*>(fileData.c_str()), fileData.size());
	std::string fileSizeBeforeEncryption = intToHexLittleEndian(fileData.size(), 4);
	std::string encryptedFile = byteToHex(aes.encrypt(fileData.c_str(), fileData.size()));
	std::string fileSizeAfterEncryption = intToHexLittleEndian(encryptedFile.size(), 4);

	payload = fileSizeAfterEncryption + fileSizeBeforeEncryption + intToHexLittleEndian(packetNum, 2) + intToHexLittleEndian(totalPackets, 2) + toHexString(fileName, 255) + encryptedFile;

	return payload;
}

ClientRequest::ClientRequest(const std::string& clientID, int version, int code, const std::string& userName,
	const std::string& publicKey, const std::string& fileName, int packetNum, int totalPackets, AESWrapper& aes, unsigned long& crc)
{
	this->clientID = clientID;
	this->version = intToHexLittleEndian(version, 1);
	this->code = intToHexLittleEndian(code, 2);

	switch(code){
		case REGISTRATION:
			this->clientID = intToHexLittleEndian(0, 16);
			this->payload = toHexString(userName, 255);
			this->payloadSize = intToHexLittleEndian(userName.size() * 2, 4);
			break;
		case RECCONECTION:
			this->payload = toHexString(userName, 255);
			this->payloadSize = intToHexLittleEndian(userName.size() * 2, 4);
			break;
		case PROPER_CRC:
		case NONPROPER_CRC:
		case FOURTH_NONPROPER_CRC:
			this->payload = toHexString(fileName, 255);
			this->payloadSize = intToHexLittleEndian(fileName.size() * 2, 4);
			break;
		case SEND_PUBLIC_KEY:
			this->payload = toHexString(userName, 255) + publicKey;
			this->payloadSize = intToHexLittleEndian(255 + publicKey.size(), 4);
			break;
		case SEND_FILE:
			std::string sendFilePayload = getSendFilePayload(fileName, packetNum, totalPackets, aes, crc);
			this->payload = sendFilePayload;
			this->payloadSize = intToHexLittleEndian(sendFilePayload.size(), 4);
			break;
	}
}

std::string ClientRequest::toString()
{
	return this->clientID + this->version + this->code + this->payloadSize + this->payload;
}