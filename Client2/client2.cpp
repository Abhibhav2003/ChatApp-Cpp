#include<iostream>
#include<string>
#include<conio.h>
#include<thread>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

bool Initialize()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		cout << "WSAStartup failed: " << result << endl;
		return false;
	}
	return true;
}

void sendMessage(SOCKET sock)
{
	string name, message;
	cout << "Enter your name : ";
	getline(cin, name);
	while (1) {
		cout << "( " << name << " )" << "(ESC to exit): ";
		char ch = _getch();
		if (ch == 27)
		{
			cout << "Stopping the Application" << endl;
			break;
		}
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent = send(sock, msg.c_str(), msg.size(), 0);

		if (bytesent == SOCKET_ERROR) {
			cout << "Error sending message: " << WSAGetLastError() << endl;
			break;
		}
	}
	closesocket(sock);
	WSACleanup();
}

void receiveMessage(SOCKET sock) {
	char buffer[4096];
	int receiveLength;
	string msg = "";
	while (1) {
		receiveLength = recv(sock, buffer, sizeof(buffer), 0);
		if (receiveLength <= 0)
		{
			cout << "Connection closed or error occurred: " << WSAGetLastError() << endl;
			break;
		}
		else {
			msg = string(buffer, receiveLength);
			cout << msg << endl;
		}
	}
	closesocket(sock);
	WSACleanup();
}
int main()
{
	if (!Initialize())
	{
		cout << "Failed to initialize Winsock." << endl;
		return 1;
	}

	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cout << "Socket creation failed: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}
	sockaddr_in serveraddr;
	int port = 12345;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port); // Port number
	string serveraddress = "127.0.0.1";
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr)); // IP address

	if (connect(sock, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		cout << "Connection failed: " << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	cout << "Connected to server." << endl;

	thread senderthread(sendMessage, sock);
	thread recieverthread(receiveMessage, sock);

	senderthread.join();
	recieverthread.join();

	return 0;
}



