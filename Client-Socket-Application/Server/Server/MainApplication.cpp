/*
	Server
*/

#pragma comment(lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS // for depricated API

#include <WinSock2.h>
#include <iostream>

using namespace std;

SOCKET Connections[100];
int ConnectionCounter = 0;

enum Packet
{
	Packet_ChatMessage,
	P_Test
};

bool SendInt(int index, int _int)
{
	int returnCheck = send(Connections[index], (char*)&_int, sizeof(int), NULL); // sends integer
	if (returnCheck == SOCKET_ERROR) // If int failed to send the connectoin 
	{
		return false; // didn't get the integer 
	}
	else
	{
		return true; //  Sucessful in return the int
	}
}
bool GetInt(int index, int &_int
)
{
	int returnCheck = recv(Connections[index], (char*)&_int, sizeof(int), NULL); // receives integer
	if (returnCheck == SOCKET_ERROR) // If int failed to send the connectoin 
	{
		return false; // didn't get the integer 
	}
	else
	{
		return true; //  Sucessful in return the int
	}
}

bool SendPacketType(int index, Packet _packetType)
{
	int returnCheck = send(Connections[index], (char*)&_packetType, sizeof(Packet), NULL); // sends packetT type
	if (returnCheck == SOCKET_ERROR) // If packet failed to send due to connection issues
	{
		return false; // didn't get the integer 
	}
	else
	{
		return true; //  Sucessful in return the int
	}
}

bool GetPacketType(int index, Packet _packetType)
{
	int returnCheck = recv(Connections[index], (char*)&_packetType, sizeof(Packet), NULL); // gets packetT type
	if (returnCheck == SOCKET_ERROR) // If packet failed to send due to connection issues
	{
		return false; // didn't get the integer 
	}
	else
	{
		return true; //  Sucessful in return the int
	}
}

bool SendString(int index, string &_string)
{
	if (!SendPacketType(index, Packet_ChatMessage))
	{
		return false;
	}
	int bufferLength = _string.size();
	if (!SendInt(index, bufferLength))
	{
		return false;
	}
	int returnCheck = send(Connections[index], _string.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool GetString(int index, string &_string)
{
	int bufferLength;
	if (!GetInt(index,bufferLength))
		return false;
	char * buffer = new char[bufferLength + 1];
	buffer[bufferLength] = '\0';
	int renCheck = recv(Connections[index], buffer, bufferLength, NULL);
	_string = buffer;
	delete[] buffer;
	if (renCheck == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}


bool ProcessPacket(int index, Packet packetType)
{
	int bufferLength;
	switch (packetType)
	{
	case Packet_ChatMessage:
	{
		string Message;
		INT numberOfConnections = 100;
		if (!GetString(index, Message))
		{
			return false;
		}

		for (int i = 0; i < numberOfConnections; i++)
		{
			if (i == index)
			{
				continue;
			}
			if (!SendString(i, Message))
			{
				cout << "Failed to send message from client ID: " << index << " to " << i << endl;
			}
		}
		cout << "Processed chat message packet from user ID: " << index << endl;
		break;
	}
	default:
		cout << "Unrecognized packet: " << packetType << endl;
		break;
	}
	return true;
}


void ClientHandlerThread(int index)
{
	
	while (true)
	{
		Packet packetType;

		recv(Connections[index], (char*)&packetType, sizeof(Packet), NULL); // Receive the type of packet
		if (!(GetPacketType(index, packetType)))
		{
			break;
		}
		if (!ProcessPacket(index, packetType))
		{
			break;
		}
	}
	cout << "Lost connection to client ID: " << index << endl;
	closesocket(Connections[index]);
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
	int addrlen = sizeof(addr); //length of address (required for accept call)
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Broadcast locally
	addr.sin_port = htons(1111); //Port
	addr.sin_family = AF_INET; //IPV4 socket

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); // Creates a scoket to listen for connections
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); // bind address the socket
	listen(sListen, SOMAXCONN);

	SOCKET newConnection; //Socket to hold the client's connection
	int numberOfConnections = 100;
	for (int i = 0; i < numberOfConnections; i++) // Accepts up to the number of connections determined by numberOfConnections
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Accept a new connection

		if (newConnection == 0)
		{
			std::cout << "Failed to accept the client's connection" << std::endl;
		}
		else //If clients connection is accepted
		{
			std::cout << "Client connected" << std::endl;
			Connections[i] = newConnection;
			numberOfConnections++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL); //Create thread to handle client 
			string outputMessage = "Welcome! You are a great programmer"; // Craetes a buffer with a message.
			int outputMessageSize = outputMessage.size();
			SendString(i, outputMessage);
		}
	}

	system("pause");

	return 0;
}