#include "client.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include "Base64Wrapper.h"

void client()
{
	ClientRequest clientRequest;
	ServerAnswer serverAnswer;
	std::string req;
	std::string ans;
	std::string address, port, clientName, filePathForSending;
	std::string clientID, privateKey, publicKey;
	RSAPrivateWrapper rsapriv;
	bool meInfoExists = false;
	int packetNum = 0, totalPackets = 0;
	std::string aesKey;
	AESWrapper aes;
	std::string temp;
	unsigned long crc;

	getTransferInfo(address, port, clientName, filePathForSending);

	if (std::filesystem::exists("me.info"))
	{
		getMeInfo(clientName, clientID, privateKey);

		if (address == "" || port == "" || clientName == "" || filePathForSending == "" || clientID == "" || privateKey == "")
		{
			std::cerr << std::endl << "Can't get the needed info!" << std::endl;
			return;
		}
		
		meInfoExists = true;
	}
	else
	{
		if (address == "" || port == "" || clientName == "" || filePathForSending == "")
		{
			std::cerr << std::endl << "Can't get the transfer info!" << std::endl;
			return;
		}
	}

	// connecting to the server
	boost::asio::io_context io_context;
	tcp::socket sock(io_context);
	tcp::resolver resolver(io_context);
	boost::asio::connect(sock, resolver.resolve(address, port));

	if (meInfoExists)
	{
		new (&rsapriv) RSAPrivateWrapper(Base64Wrapper::decode(privateKey));
		RSAPublicWrapper rsapub(rsapriv.getPublicKey());
		// Request for recconection
		totalPackets++;
		packetNum++;
		clientRequest = ClientRequest(clientID, CLIENT_VERSION, RECCONECTION, clientName, publicKey, filePathForSending, packetNum, totalPackets, aes, crc);
		req = clientRequest.toString();
		std::cout << std::endl << "Sending recconection request to server." << std::endl;
		ans = request(sock, req, "Sending recconection request again to server.", "Can not recconnect to server", serverAnswer);
		if (ans == "fatal")
			return;
		if (ans != "recconection failed")
		{
			temp = hexToBase64(serverAnswer.getPayload().substr(32));
			aesKey = rsapriv.decrypt(Base64Wrapper::decode(temp));
			new (&aes) AESWrapper(aesKey);

			// request for sending file
			totalPackets++;
			packetNum++;
			clientRequest = ClientRequest(clientID, CLIENT_VERSION, SEND_FILE, clientName, publicKey, filePathForSending, packetNum, totalPackets, aes, crc);
			ClientRequest CRCNotProper(clientID, CLIENT_VERSION, NONPROPER_CRC, clientName, publicKey, filePathForSending, packetNum, totalPackets, aes, crc);
			ClientRequest CRCNotProperFourthTime(clientID, CLIENT_VERSION, FOURTH_NONPROPER_CRC, clientName, publicKey, filePathForSending, packetNum, totalPackets, aes, crc);
			req = clientRequest.toString();
			std::string crcReq = CRCNotProper.toString();
			std::string crcFourthReq = CRCNotProperFourthTime.toString();
			std::cout << std::endl << "Sending the file " << filePathForSending << "to server." << std::endl;
			ans = sendFileRequest(sock, req, crcReq, crcFourthReq, "CRC isn't proper sending the file again.", "CRC isn't proper for the fourth time", serverAnswer, crc);

			return; // end the connection
		}
	}
	
	privateKey = rsapriv.getPrivateKey();
	// create priv.key ///////////////////////////////////////////////////////////
	std::ofstream privKeyFile("priv.key");

	if (privKeyFile.is_open())
	{
		privKeyFile << hexToBase64(byteToHex(privateKey));
	}
	else
	{
		std::cerr << std::endl << "Unable to create priv.key file!" << std::endl;
		return;
	}
	privKeyFile.close();
	/////////////////////////////////////////////////////////////////////////////
	publicKey = toHexString(rsapriv.getPublicKey(), 160);
	RSAPublicWrapper rsapub(rsapriv.getPublicKey());
	
	// Request for registration
	totalPackets++;
	packetNum++;
	clientRequest = ClientRequest(clientID, CLIENT_VERSION, REGISTRATION, clientName, publicKey, filePathForSending, packetNum, totalPackets, aes, crc);
	req = clientRequest.toString();
	std::cout << std::endl << "Sending registration request to server." << std::endl;
	ans = request(sock, req, "Sending registration request again to server.", "Can not register to server", serverAnswer);
	if (ans == "fatal")
		return;
	clientID = serverAnswer.getPayload();


	// Sending public key
	totalPackets++;
	packetNum++;
	clientRequest = ClientRequest(clientID, CLIENT_VERSION, SEND_PUBLIC_KEY, clientName, publicKey, filePathForSending, packetNum, totalPackets, aes, crc);
	req = clientRequest.toString();
	std::cout << std::endl << "Sending public key to server." << std::endl;
	ans = request(sock, req, "Sending the public key again to server.", "Can not send the public key to server", serverAnswer);
	if (ans == "fatal")
		return;
	temp = hexToBase64(serverAnswer.getPayload().substr(32));
	aesKey = rsapriv.decrypt(Base64Wrapper::decode(temp));
	new (&aes) AESWrapper(aesKey);


	// Creating me.info file
	std::ofstream meInfoFile("me.info");

	if (!meInfoFile)
	{
		std::cout << std::endl << "can not create me.info file! exiting..." << std::endl;
		return;
	}

	meInfoFile << clientName << std::endl;
	meInfoFile << clientID << std::endl;
	meInfoFile << hexToBase64(byteToHex(privateKey)) << std::endl;

	// request for sending file
	totalPackets++;
	packetNum++;
	clientRequest = ClientRequest(clientID, CLIENT_VERSION, SEND_FILE, clientName, publicKey, filePathForSending, packetNum, totalPackets, aes, crc);
	ClientRequest CRCNotProper(clientID, CLIENT_VERSION, NONPROPER_CRC, clientName, publicKey, filePathForSending, packetNum, totalPackets, aes, crc);
	ClientRequest CRCNotProperFourthTime(clientID, CLIENT_VERSION, FOURTH_NONPROPER_CRC, clientName, publicKey, filePathForSending, packetNum, totalPackets, aes, crc);
	std::string crcReq = CRCNotProper.toString();
	std::string crcFourthReq = CRCNotProperFourthTime.toString();
	req = clientRequest.toString();
	std::cout << std::endl << "Sending the file " << filePathForSending << " to server." << std::endl;
	ans = sendFileRequest(sock, req, crcReq, crcFourthReq, "CRC isn't proper sending the file again.", "CRC isn't proper for the fourth time", serverAnswer, crc);
}

int main()
{
	client();
	
	return 0;
}