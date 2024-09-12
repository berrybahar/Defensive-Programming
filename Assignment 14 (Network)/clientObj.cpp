#include "clientObj.h"

Client::Client(std::string clientRequest)
{
	this->_userId = clientRequest.substr(0, 8);
	this->_version = clientRequest.substr(8, 2);
	this->_op = clientRequest.substr(10, 2);

    if (hexStringToNumber(this->_op) == SAVE_FILE)
    {
        this->_nameLen = clientRequest.substr(12, 4);
        this->_filename = clientRequest.substr(16, hexStringToNumber(this->_nameLen)*2);

        this->_size = clientRequest.substr(16 + hexStringToNumber(this->_nameLen)*2, 8);
        this->_payload = clientRequest.substr(16 + (hexStringToNumber(this->_nameLen)*2) + 8);
    }
    else if (hexStringToNumber(this->_op) == RETRIEVE_FILE || hexStringToNumber(this->_op) == DELETE_FILE)
    {
        this->_nameLen = clientRequest.substr(12, 4);
        this->_filename = clientRequest.substr(16, hexStringToNumber(this->_nameLen)*2);

        this->_size = "";
        this->_payload = "";
    }
    else if (hexStringToNumber(this->_op) == RETURN_CLIENT_FILE_LIST)
    {
        this->_nameLen = "";
        this->_filename = "";
        this->_size = "";
        this->_payload = "";
    }
}

Client::~Client()
{
    this->_userId.clear();
    this->_version.clear();
    this->_op.clear();
    this->_nameLen.clear();
    this->_filename.clear();
    this->_size.clear();
    this->_payload.clear();
}

std::string deleteNullString(const std::string& str)
{
    std::string res;

    for (char c : str)
        if (c != '\0') // Skip null characters
            res += c;

    return res;
}

std::string byteStringToHexString(const std::string& byteStr)
{
    std::ostringstream hexStream;
    for (unsigned char byte : byteStr)
        hexStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return hexStream.str();
}

std::string intToHex(int value, size_t byteSize) 
{
    std::ostringstream oss;
    // Set the stream to output hex with zero padding
    oss << std::hex << std::setfill('0');

    // Extract each byte in little-endian order and append to the stream
    for (size_t i = 0; i < byteSize; ++i) 
    {
        int byte = (value >> (i * 8)) & 0xFF; // Extract the i-th byte
        oss << std::setw(2) << byte; // Output as 2-digit hex
    }

    return oss.str();
}

std::string stringToHex(const std::string& input) 
{
    std::ostringstream oss;
    // Set the stream to output hex with zero padding
    oss << std::hex << std::setfill('0');

    // Iterate through each character in the input string
    for (unsigned char c : input)
        oss << std::setw(2) << static_cast<int>(c); // Output each char as 2-digit hex

    return oss.str();
}

std::vector<unsigned char> hexStringToBytes(const std::string& hexStr) 
{
    std::vector<unsigned char> bytes;
    size_t len = hexStr.length();

    // Ensure the hex string has an even length
    if (len % 2 != 0) 
    {
        std::cerr << "Hex string length must be even." << std::endl;
        return bytes;
    }

    for (size_t i = 0; i < len; i += 2) 
    {
        // Convert each pair of hex digits to a byte
        std::string byteString = hexStr.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}

std::string generateRandomString(size_t length) 
{
    const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::random_device rd;  // for random number engine
    std::mt19937 generator(rd());  // random number engine
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string randomString;
    for (size_t i = 0; i < length; ++i)
        randomString += characters[distribution(generator)];

    return randomString;
}

uint64_t hexStringToNumber(const std::string& hexString) 
{
    // Ensure the hex string length is even
    if (hexString.length() % 2 != 0) 
        throw std::invalid_argument("Hex string length must be even.");

    // Convert hex string to byte array
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hexString.length(); i += 2) 
    {
        std::string byteString = hexString.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }

    // Combine bytes into a number (assuming 64-bit unsigned integer)
    uint64_t number = 0;
    for (size_t i = 0; i < bytes.size(); ++i)
        number |= static_cast<uint64_t>(bytes[i]) << (i * 8);

    return number;
}

std::string hexStringToString(const std::string& hex)
{
    std::string result;
    if (hex.length() % 2 != 0)
        throw std::invalid_argument("Hex string must have an even length.");

    // Iterate over the hex string in pairs of two characters
    for (size_t i = 0; i < hex.length(); i += 2) 
    {
        // Extract the two characters
        std::string byteString = hex.substr(i, 2);

        // Convert the two characters to a single byte
        char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));

        // Append the byte to the result string
        result += byte;
    }

    return result;
}

std::vector<std::string> listFiles(const std::string& path)
{
    std::vector<std::string> fileList;
    try
    {
        // Check if the path exists and is a directory
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
        {
            // Iterate over the directory entries
            for (const auto& entry : std::filesystem::directory_iterator(path))
                fileList.emplace_back(entry.path().filename().string());
        }
        else
        {
            std::cerr << "The path is not a directory or does not exist." << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return fileList;
}

std::string saveFile(Client client, std::string backupFileDirectory)
{
    std::string fileName = hexStringToString(client.getFileName());
    std::string filePath = backupFileDirectory + '\\' + fileName;
    // Convert hex string to binary data
    std::vector<unsigned char> binaryData = hexStringToBytes(client.getPayLoad());
    std::ofstream file(filePath, std::ios::binary);

    if (!file)
    {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return intToHex(SERVER_VERSION, 1) + intToHex(SERVER_PROBLEM, 2);
    }

    // Write the binary data to the file
    file.write(reinterpret_cast<const char*>(binaryData.data()), binaryData.size());

    file.close();

    std::cout << "\nSaved file-" + hexStringToString(client.getFileName()) + " to the directory of client-" +
        std::to_string(hexStringToNumber(client.getUserId())) << std::endl;

    return intToHex(SERVER_VERSION, 1) + intToHex(FILE_SAVED_OR_DELETED, 2) +
        intToHex(fileName.length(), 2) + stringToHex(deleteNullString(fileName));
}

std::string retrieveFile(Client client, std::string backupFileDirectory, std::string clientFilePath)
{
    std::string retrieveFileDirectory = backupFileDirectory + '\\' + hexStringToString(client.getFileName());
    std::string tmpPath = clientFilePath + '\\' + "tmp";

    if (std::filesystem::exists(retrieveFileDirectory))
    {
        std::ifstream file(retrieveFileDirectory, std::ios::binary);
        
        if (!file)
        {
            std::cerr << "Error opening file: " << client.getFileName() << std::endl;
            return intToHex(SERVER_VERSION, 1) + intToHex(SERVER_PROBLEM, 2);
        }

        std::ostringstream ostrm;
        ostrm << file.rdbuf();
        file.close();

        // you have function for bytes string to hex string and then hex string to bytes string also
        std::ofstream tmpFile(tmpPath, std::ios::binary);
        
        if (!tmpFile)
        {
            std::cerr << "Error opening file: tmp"<< std::endl;
            return intToHex(SERVER_VERSION, 1) + intToHex(SERVER_PROBLEM, 2);
        }

        std::vector<unsigned char> binaryData = hexStringToBytes(byteStringToHexString(ostrm.str()));

        // Write the binary data to the file
        tmpFile.write(reinterpret_cast<const char*>(binaryData.data()), binaryData.size());
        tmpFile.close();

        std::uintmax_t fileSize = std::filesystem::file_size(retrieveFileDirectory);

        std::cout << "\nRetrieved file-" + hexStringToString(client.getFileName()) + " to client-" +
            std::to_string(hexStringToNumber(client.getUserId())) << std::endl;

        return intToHex(SERVER_VERSION, 1) + intToHex(FILE_RETRIEVED, 2) +
            intToHex(client.getFileName().length(), 2) + stringToHex(deleteNullString(client.getFileName())) +
            intToHex(fileSize, 4) + byteStringToHexString(ostrm.str());
    }

    std::cout << "\nFile doesn't exist! Can not retrieve file-" + hexStringToString(client.getFileName()) << std::endl;

    return intToHex(SERVER_VERSION, 1) + intToHex(FILE_NOT_EXIST, 2);
}

std::string deleteFile(Client client, std::string backupFileDirectory)
{
    std::string fileName = hexStringToString(client.getFileName());
    std::string filePath = backupFileDirectory + '\\' + fileName;
    
    if (std::filesystem::exists(filePath))
    {
        std::filesystem::remove(filePath);

        std::cout << "\nDeleted file-" + hexStringToString(client.getFileName()) + " from the directory of client-" +
            std::to_string(hexStringToNumber(client.getUserId())) << std::endl;

        return intToHex(SERVER_VERSION, 1) + intToHex(FILE_SAVED_OR_DELETED, 2) +
            intToHex(fileName.length(), 2) + stringToHex(deleteNullString(fileName));
    }

    std::cout << "\nFile doesn't exist! Can not delete file-" + hexStringToString(client.getFileName()) << std::endl;

    return intToHex(SERVER_VERSION, 1) + intToHex(FILE_NOT_EXIST, 2);
}

std::string getClientFileList(Client client, std::string backupFileDirectory, std::string clientFilePath)
{
    std::vector<std::string> fileList = listFiles(backupFileDirectory);

    if (!fileList.empty())
    {
        // create random name for the list file
        std::string listFileName = generateRandomString(32);
        std::ofstream file;
        std::string filePath = clientFilePath + '\\' + listFileName;
        file.open(filePath);

        if (!file)
        {
            std::cerr << "Error opening file: " << listFileName << std::endl;
            return intToHex(SERVER_VERSION, 1) + intToHex(SERVER_PROBLEM, 2);
        }

        for (int i = 0; i < fileList.size(); i++)
            file << fileList[i] << std::endl;
        file.close();

        std::uintmax_t fileSize = std::filesystem::file_size(filePath);

        std::ifstream binaryFile(filePath, std::ios::binary);

        if (!binaryFile)
        {
            std::cerr << "Error opening file: " << listFileName << std::endl;
            return intToHex(SERVER_VERSION, 1) + intToHex(SERVER_PROBLEM, 2);
        }

        std::stringstream buffer;
        // Read the file content into the string stream
        buffer << binaryFile.rdbuf();

        binaryFile.close();

        std::cout << "\nSending directory of files to Client-" + std::to_string(hexStringToNumber(client.getUserId())) << std::endl;

        return intToHex(SERVER_VERSION, 1) + intToHex(RETURN_CLIENT_FILE_LIST, 2) +
            intToHex(listFileName.length(), 2) + stringToHex(deleteNullString(listFileName)) +
            intToHex(fileSize, 4) + stringToHex(buffer.str());
    }

    std::cout << "\nCLient-" + std::to_string(hexStringToNumber(client.getUserId())) +
        "don't have files in the backup directory! Can not delete file-" + hexStringToString(client.getFileName()) << std::endl;

    return intToHex(SERVER_VERSION, 1) + intToHex(CLIENT_NOT_HAVE_FILE, 2);
}

void answer(Client client, std::string& answerToClient, std::string backupFileDirectory, std::string clientFilePath)
{
    uint64_t op = hexStringToNumber(client.getOp());

    if (op == SAVE_FILE)
    {
        std::cout << "\nClient-" + std::to_string(hexStringToNumber(client.getUserId())) + " wants to save file-" + hexStringToString(client.getFileName()) << std::endl;
        answerToClient = saveFile(client, backupFileDirectory);
    }
    else if (op == RETRIEVE_FILE)
    {
        std::cout << "\nClient-" + std::to_string(hexStringToNumber(client.getUserId())) + " wants to retrieve file-" + hexStringToString(client.getFileName()) << std::endl;
        answerToClient = retrieveFile(client, backupFileDirectory, clientFilePath);
    }
    else if (op == DELETE_FILE)
    {
        std::cout << "\nClient-" + std::to_string(hexStringToNumber(client.getUserId())) + " wants to delete file-" + hexStringToString(client.getFileName()) << std::endl;
        answerToClient = deleteFile(client, backupFileDirectory);
    }
    else if (op == GET_CLIENT_FILE_LIST)
    {
        std::cout << "\nClient-" + std::to_string(hexStringToNumber(client.getUserId())) + " wants to get the directory of files" << std::endl;
        answerToClient = getClientFileList(client, backupFileDirectory, clientFilePath);
    }
}