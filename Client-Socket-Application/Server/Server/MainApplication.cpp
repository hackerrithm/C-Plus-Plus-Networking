/*
	Server
*/

#pragma comment(lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS // for depricated API

#include <WinSock2.h>
#include <iostream>

using namespace std;

int main()
{
	// Winsock start up
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) // if startup returns anything other than 0 then something is wrong
	{
		MessageBox(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	SOCKADDR_IN addr; //Address that we will bind our listening socket to
	int addrlen = sizeof(addr); //length of address (required for accept call)
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Broadcast locally
	addr.sin_port = htons(1111); //Port
	addr.sin_family = AF_INET; //IPV4 socket

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); // Creates a scoket to listen for connections
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); // bind address the socket
	listen(sListen, SOMAXCONN);

	SOCKET newConnection; //Socket to hold the client's connection
	newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Accept a new connection

	if (newConnection == 0) 
	{
		std::cout << "Failed to accept the client's connection" << std::endl;
	}
	else //If clients connection is accepted
	{
		std::cout << "Client connected" << std::endl;
	}

	system("pause");

	return 0;
}