#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

/*
* 7 Steps for Server Functions
* 1- Initialize WSA - WSAStartup(),
* 2- Create a socket - socket(),
* 3- Bind the Socket - bind(),
* 4- Listen on the socket - listen(),
* 5- Accept a connection - accept(),
* 6- Send and receive data - recv(), send() (for TCP),
* 7- Disconnect - closesocket().
*/

int main()
{
	// Step - 1 ||| Initialize WSA - WSAStartup();
	SOCKET serverSocket;
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
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "socket() is OK!" << std::endl;
	}

	// Step - 3 ||| Bind the socket() - bind()
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
	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(serverService.sin_addr));
	serverService.sin_port = htons(port);
	if (bind(serverSocket, (SOCKADDR*)&serverService, sizeof(serverService)) == SOCKET_ERROR) {
		std::cerr << "bind() failed! Error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "bind() is OK!" << std::endl;
	}

	// Step - 4 ||| Listen on the socket - listen()
	if (listen(serverSocket, 1) == SOCKET_ERROR) {
		std::cerr << "Error listening on socket with this error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "listen() is OK! Waiting for connection.." << std::endl;
	}

	// Step - 5 ||| Accept a connection - accept()
	SOCKET acceptSocket = accept(serverSocket, NULL, NULL);
	if (acceptSocket == INVALID_SOCKET) {
		std::cerr << "Accept failed with this error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "Accepted connection" << std::endl;
	}

	// Step - 6 ||| Send and receive data - recv(), send()
	while (true) {
		// Receive Message from Client
		char buffer[1024];
		int bytesReceived = recv(acceptSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived > 0) {
			buffer[bytesReceived] = '\0';
			std::cout << "Received data from client: " << buffer << std::endl;
		}
		else if (bytesReceived == 0) {
			std::cout << "Connection closed by client. " << std::endl;
			break;
		}
		else {
			std::cerr << "Recv failed with error: " << WSAGetLastError() << std::endl;
			break;
		}

		// Send message to the Client.
		std::string message;
		std::cout << "Enter your message: ";
		std::getline(std::cin, message);

		int byteCount = send(acceptSocket, message.c_str(), message.size(), 0);
		if (byteCount == SOCKET_ERROR) {
			std::cerr << "Client send error: " << WSAGetLastError() << std::endl;
			break;
		}
	}

	// Step - 7 ||| Close the socket - closesocket(SOCKET)
	closesocket(serverSocket);
	closesocket(acceptSocket);
	WSACleanup();

	return 0;
}