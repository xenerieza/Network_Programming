#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

/*
* 5 steps for Server Functions for UDP
* 1- Initialize WSA - WSAStartup(),
* 2- Create a socket - socket(),
* 3- Bind the Socket - bind(),
* 4- Send and receive data - recvfrom() | sendto(),
* 5- Disconnect - closesocket().
*/


int main()
{
	// Step - 1 ||| Initialize WSA - WSAStartup()
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	int port = 8080;
	int wsaerr = WSAStartup(wVersionRequested, &wsaData);
	if (wsaerr != 0) {
		std::cerr << "The winsock dll not found!" << std::endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "The winsock dll found!" << std::endl;
		std::cout << "The Status: " << wsaData.szSystemStatus << std::endl;
	}

	// Step - 2 ||| Create a socket - socket()
	/*
	* AF_INET --> IPv4 address family,
	* SOCK_DGRAM --> UDP,
	* IPPROTO_UDP --> UDP.
	*/

	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "Server socket() is OK!" << std::endl;
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


	// Step - 4 ||| Send and Receive data - recvfrom(), sendto()
	while (true) {
		//Receive Message from Client
		char buffer[1024];
		sockaddr_in serverAddr;
		int serverAddrSize = sizeof(serverAddr);
		int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0,
			(SOCKADDR*)&serverAddr, &serverAddrSize);
		if (bytesReceived < 0) {
			std::cerr << "Recvfrom failed with error: " << WSAGetLastError() << std::endl;
		}
		else if (bytesReceived == 0) {
			std::cout << "Connection closed by peer." << std::endl;
		}
		else {
			buffer[bytesReceived] = '\0';
			std::cout << "Received Data: " << buffer << std::endl;
		}

		// Send Message to the Client
		std::string message;
		std::cout << "Enter your message: ";
		std::getline(std::cin, message);

		int byteCount = sendto(serverSocket, message.c_str(), message.size(), 0,
			(SOCKADDR*)&serverAddr, sizeof(serverAddr));
		if (byteCount == SOCKET_ERROR) {
			std::cout << "Server send to error: " << WSAGetLastError() << std::endl;
			closesocket(serverSocket);
			WSACleanup();
			exit(EXIT_FAILURE);
		}
		else {
			std::cout << "Data send successfully. Sent " << byteCount << " bytes." << std::endl;
		}
	}

	// Step - 5 ||| Disconnect - closesocket(SOCKET).
	closesocket(serverSocket);
	WSACleanup();

	return 0;


}