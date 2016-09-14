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
	P_Test
};


bool SendInt(int _int)
{
	int returnCheck = send(Connection, (char*)&_int, sizeof(int), NULL); // sends integer
	if (returnCheck == SOCKET_ERROR) // If int failed to send the connectoin 
	{
		return false; // didn't get the integer 
	}
	else
	{
		return true; //  Sucessful in return the int
	}
}

bool GetInt(int &_int)
{
	int returnCheck = recv(Connection, (char*)&_int, sizeof(int), NULL); // receives integer
	if (returnCheck == SOCKET_ERROR) // If int failed to send the connectoin 
	{
		return false; // didn't get the integer 
	}
	else
	{
		return true; //  Sucessful in return the int
	}
}

bool SendPacketType(Packet _packetType)
{
	int returnCheck = send(Connection, (char*)&_packetType, sizeof(Packet), NULL); // sends packetT type
	if (returnCheck == SOCKET_ERROR) // If packet failed to send due to connection issues
	{
		return false; // didn't get the integer 
	}
	else
	{
		return true; //  Sucessful in return the int
	}
}

bool GetPacketType(Packet &_packetType)
{
	int returnCheck = recv(Connection, (char*)&_packetType, sizeof(Packet), NULL); // gets packetT type
	if (returnCheck == SOCKET_ERROR) // If packet failed to send due to connection issues
	{
		return false; // didn't get the integer 
	}
	else
	{
		return true; //  Sucessful in return the int
	}
}

bool SendString(string &_string)
{
	if (!SendPacketType(Packet_ChatMessage))
	{
		return false;
	}
	int bufferLength = _string.size();
	if (!SendInt(bufferLength))
	{
		return false;
	}
	int returnCheck = send(Connection, _string.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool GetString(string &_string)
{
	int bufferLength;
	if (!GetInt(bufferLength))
	{
		return false;
	}
	char* buffer = new char[bufferLength + 1];
	buffer[bufferLength] = '\0';
	int renCheck = recv(Connection, buffer, bufferLength, NULL);
	_string = buffer;
	delete[] buffer;
	if (renCheck == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}


bool ProcessPacket(Packet packetType)
{
	switch (packetType)
	{
	case Packet_ChatMessage:
	{
		string message;
		if (!GetString(message))
		{
			return false;
		}
		cout << message << endl;
		break;
	}	
	default:
		cout << "Unrecognized packet: " << packetType << endl;
		break;
	}
	return true;
}

void ClientThread()
{ 
	while (true)
	{
		Packet packetType;
		//recv(Connection, (char*)&packettype, sizeof(Packet), NULL); // Receive the type of packet
		if (!(GetPacketType(packetType)))
			break;

		if (!ProcessPacket(packetType))
			break;
	}
	cout << "Lost Connection to Server" << endl;
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
	string userInput;
	while (true)
	{
		getline(cin, userInput);
		if (!SendString(userInput))
		{
			break;
		}
		Sleep(10);
	}

	system("pause");
	return 0;
}