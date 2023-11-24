#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

/* 5 Steps for Client Functions
* 1- Initialize WSA - WSAStartup(),
* 2- Create a socket - socket(),
* 3- Connect to the server - connect(),
* 4- Send and receive data - recv(), send() (for TCP),
* 5- Disconnect - closesocket(SOCKET).
*/

int main()
{
	// Step - 1 ||| Initialize WSA - WSAStartup()
	SOCKET clientSocket;
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	int port = 8080;
	int wsaerr = WSAStartup(wVersionRequested, &wsaData);
	if (wsaerr != 0) {
		std::cerr << "The Winsock dll not found!" << std::endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "The winsock dll found!" << std::endl;
		std::cout << "The Status: " << wsaData.szSystemStatus << std::endl;
	}

	// Step - 2 ||| Create a socket - socket()
	/*
	* AF_INET --> IPv4 address family.
	* SOCK_STREAM --> TCP,
	* IPPROTO_TCP --> TCP protocol.
	*/

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "socket() is OK!" << std::endl;
	}

	// Step - 3 ||| Connect to the server - connect()
	/*
	* sockaddr is a structural type and contains port informations. These are:
	* sin_family: Specifies the address family:
	*	AF_INET: IPv4,
	*	AF_INET6: IPv6.
	* sin_port: Specifies the port number to connect to.
	* The host byte order must be converted to network byte order with the 'htons' function.
	* sin_addr: Specifies the IP address to connect to. It is of type 'IN_ADDR'.
	* 'IN_ADDR': It is a structure that represents the IPv4 address.
	*/

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(clientService.sin_addr));
	clientService.sin_port = htons(port);
	if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		std::cerr << "Failed to connect with this error: " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "connect() is OK!" << std::endl;
		std::cout << "Client: Can start sending and receiving data.." << std::endl;
	}

	// Step - 4 ||| Send and receive data - recv(), send()
	while (true) {
		// Send message to the Server.
		std::string message;
		std::cout << "Enter your message for Server: ";
		std::getline(std::cin, message);
		int _size = (int)message.size();
		int byteCount = send(clientSocket, message.c_str(), _size, 0);
		if (byteCount == SOCKET_ERROR) {
			std::cerr << "Server send error: " << WSAGetLastError() << std::endl;
		}

		// Receive Message from Server.
		char buffer[1024];
		int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived > 0) {
			buffer[bytesReceived] = '\0';
			std::cout << "Received data from Server: " << buffer << std::endl;
		}
		else if (bytesReceived == 0) {
			std::cout << "Connection closed by Server. " << std::endl;
			break;
		}
		else {
			std::cerr << "Recv failed with error: " << WSAGetLastError() << std::endl;
			break;
		}
	}

	// Step - 5 ||| Close the socket - closesocket(SOCKET)
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}