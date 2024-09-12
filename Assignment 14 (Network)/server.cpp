#include "server.h"

void session(tcp::socket sock)
{
    try
    {
        int i = 1;
        uint64_t user_id;
        std::string clientFilePath;
        std::string backupDirectory;
        std::string answerToClient;
        for (;;)
        {
            std::string data = readData(sock);

            if (data == "EXIT")
            {
                std::cout << "\nClosing Connection." << std::endl;
                break;
            }

            if (i == 1)
            {
                clientFilePath = data; // getting the path to directory that the client program runs
                i += 1;
                data.clear();
                continue;
            }
            else if (i == 2)
            {
                user_id = stoll(data);
                std::cout << "\nclient-" + std::to_string(user_id) + " connected" << std::endl;
                backupDirectory = createBackupDirectory(user_id); // creating backup directory
                i += 1;
                data.clear();
                continue;
            }

            Client client = Client(data);

            // clear the data that we don't need it
            data.clear();

            std::thread t([&]()
                {
                    try
                    {
                        answer(client, answerToClient, backupDirectory, clientFilePath);
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "Exception in answer thread: " << e.what() << std::endl;
                    }
                });

            t.join();
            // send data to client
            send_data(sock, answerToClient);

            answerToClient.clear();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << std::endl;
    }
}

void server(boost::asio::io_context& io_context, unsigned short port)
{
	tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));

	std::cout << "Server is listening on port " << port << std::endl;

	for (;;)
	{
		std::thread(session, a.accept()).detach();
	}
}

int main(int argc, char* argv[])
{
	boost::asio::io_context io_context;

	server(io_context, std::stoi(PORT));

	return 0;
}