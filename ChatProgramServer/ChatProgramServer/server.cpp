#pragma comment(lib, "Ws2_32.lib")

#include<iostream>
#include <ws2tcpip.h>
#include <string>

using namespace std;

#define PORT 54000
#define MAXCON 10

int main()
{
	WSADATA wsaData;
	int iResult;
	SOCKET sockFD;
	int newSocket, dataRead;
	bool opt = false;
	char buffer[1024] = { 0 };

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed with error: %d\n" << iResult;
		return 0;
	}

	if ((sockFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cout << "Socket Error";
		return 0;
	}

	if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(bool)) == -1)
	{
		std::cout << "Setsockopt failed";
		return 0;
	}

	SOCKADDR_IN address;

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);

	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr.s_addr);


	int addrLen = sizeof(address);

	if (bind(sockFD, (sockaddr *)&address, addrLen) < 0)
	{
		std::cout << "Bind Failed";
	}

	if (listen(sockFD, MAXCON) == -1)
	{
		std::cout << "Listen Failed";
	}

	fd_set master;

	FD_ZERO(&master);

	FD_SET(sockFD, &master);

	while (true)
	{
		fd_set masterCopy = master;

		int socketCount = select(0, &masterCopy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = masterCopy.fd_array[i];

			if (sock == sockFD)
			{
				SOCKET client = accept(sock, nullptr, nullptr);

				FD_SET(client, &master);

				string message = "Welcome to server";
				send(client, message.c_str(), message.size() + 1, 0);
			}
			else
			{
				int byteReceived = recv(sock, buffer, 1024, 0);

				for (int i = 0; i < master.fd_count; i++)
				{
					SOCKET sendSocket = master.fd_array[i];

					if (sendSocket != sockFD && sendSocket != sock)
					{
						send(sendSocket, buffer, byteReceived, 0);
					}

				}

			}
		}
	}

	return 0;
}