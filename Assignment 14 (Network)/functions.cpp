#include "server.h"

void send_data(tcp::socket& socket, const std::string& data) {
	// Convert data size to a 4-byte unsigned integer
	uint32_t data_size = static_cast<uint32_t>(data.size());
	uint32_t size_network_order = htonl(data_size); // Convert to network byte order

	// Send the size of the data first
	boost::asio::write(socket, boost::asio::buffer(&size_network_order, sizeof(size_network_order)));

	// Send the actual data
	boost::asio::write(socket, boost::asio::buffer(data));
}

void readSize(tcp::socket& socket, char* buffer, size_t size)
{
	boost::asio::read(socket, boost::asio::buffer(buffer, size));
}

std::string readData(tcp::socket& socket)
{
	char header[4];

	try
	{
		// Read the size of the data
		readSize(socket, header, sizeof(header));

		// Convert the header to length (safely using memcpy to avoid alignment issues)
		uint32_t dataLength;
		std::memcpy(&dataLength, header, sizeof(dataLength));
		dataLength = ntohl(dataLength); // Convert from network byte order to host byte order

		// Prepare a buffer to hold the incoming data
		std::vector<char> data(dataLength);

		// Read the actual data
		boost::asio::read(socket, boost::asio::buffer(data.data(), dataLength));

		return std::string(data.begin(), data.end());
	}
	catch (const boost::system::system_error& e)
	{
		std::cerr << "Error reading data: " << e.what() << std::endl;
		return ""; // Return an empty string on error
	}
}

WCHAR* stringToWCHAR(const std::string& str) 
{
	// Get the required buffer size
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	// Allocate buffer for WCHAR*
	WCHAR* wstr = new WCHAR[size_needed];

	// Perform the conversion
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wstr, size_needed);

	return wstr;
}

std::string createBackupDirectory(uint64_t userId)
{
	// need to check for unsigned num
	std::string dirName = "C:\\backupsvr\\" + std::to_string(userId);
	std::filesystem::remove_all(dirName);
	
	// Convert to WCHAR*
	WCHAR* backupDirectory = stringToWCHAR(dirName);

	// Create a backup directory
	if (std::filesystem::create_directory(backupDirectory) || ERROR_ALREADY_EXISTS == GetLastError()) 
	{
		std::cout << "\nBackup directory created successfully or already exists." << std::endl << std::endl;
	}
	else 
	{
		std::cerr << "\nFailed to create backup directory. Error: " << GetLastError() << std::endl << std::endl;
	}

	return dirName;
}