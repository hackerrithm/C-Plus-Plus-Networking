/*
	Client
*/
#pragma comment(lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS // for depricated API

#include <WinSock2.h>
#include <iostream>
#include <string>

using namespace std;

SOCKET Connection;

enum Packet
{
	Packet_ChatMessage,
	Packet_Test
};

bool ProcessPacket(Packet packetType)
{
	switch (packetType)
	{
	case Packet_ChatMessage:
	{
		int bufferLength;
		recv(Connection, (char*)&bufferLength, sizeof(int), NULL);
		char * buffer = new char[bufferLength + 1]; // Allocating buffer
		buffer[bufferLength] = '\0';
		recv(Connection, buffer, bufferLength, NULL);
		cout << buffer << endl;

		delete[] buffer; // Deallocating buffer

		break;
	}
	case Packet_Test:
		cout << "Receieved the test packet" << endl;
		break;
	
	default:
		cout << "Unrecognized packet: " << packetType << endl;
		break;
	}
	return true;
}

void ClientThread()
{
	int bufferLength;
	Packet packetType;
	while (true)
	{
		recv(Connection, (char*)&packetType, sizeof(Packet), NULL); // Receive the type of packet

		if (!ProcessPacket(packetType))
		{
			break;
		}
	}
	closesocket(Connection);
}

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

	Connection = socket(AF_INET, SOCK_STREAM, NULL); // Creates a scoket to listen for connections
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) // If unable to connect
	{
		MessageBox(NULL, "Failed to connect", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	else
	{
		std::cout << "Connected from the client" << std::endl;
	}
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL); //Create thread to handle client 

	string buffer;
	while (true)
	{
		getline(cin, buffer);
		int bufferLength = sizeof(buffer);
		Packet packetType = Packet_ChatMessage; // Creates Packet type
		send(Connection, (char*)&packetType, sizeof(Packet), NULL);
		send(Connection, (char*)&bufferLength, sizeof(int), NULL); // send length of the message
		send(Connection, buffer.c_str(), bufferLength, NULL); // sends the message
		Sleep(10);
	}

	system("pause");
	return 0;
}