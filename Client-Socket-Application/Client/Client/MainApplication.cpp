/*
	Client
*/

#pragma comment(lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS // for depricated API

#include <WinSock2.h>
#include <iostream>

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
	int sizeofaddr = sizeof(addr); //length of address (required for accept call)
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Broadcast locally
	addr.sin_port = htons(1111); //Port
	addr.sin_family = AF_INET; //IPV4 socket

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL); // Creates a scoket to listen for connections
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) // If unable to connect
	{
		MessageBox(NULL, "Failed to connect", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	else
	{
		std::cout << "Connected from the client" << std::endl;
	}
	system("pause");
	return 0;
}