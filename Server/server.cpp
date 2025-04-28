#include<iostream>
#include<vector>
#include<Ws2tcpip.h>
#include<WinSock2.h>
#include<tchar.h>
#include<thread>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

bool Initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;

}
void InteractWithClient(SOCKET clientSocket, vector<SOCKET>& clients)
{
    // send and recieve :
    char buffer[4069];
    while (1) {
        int byteRecieved = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (byteRecieved <= 0) {
			cout << "Client Disconnected" << endl;
            break;
		}
        string message(buffer, byteRecieved);
        cout << "Recieved message : " << message << endl;

		for (auto client : clients)
		{
		if (client != clientSocket) 
        {
		    send(client, message.c_str() , message.size(), 0);
		} 
		}
    }
	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}
    closesocket(clientSocket);
}
int main()
{
    if (!Initialize()) {
        cout << "WinSock Initialization failed" << endl;
    }
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSocket == INVALID_SOCKET) {
        cout << "Socket Creation failed" << endl;
        return 1;
    }
    int port = 12345;
    //create address structure
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);


    //convert the ip address (0.0.0.0) put it inside the sin_family in binary format
    if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1)
    {
        cout << "setting address structure failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // bind :
    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
    {
		cout << "Binding failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
    }

    //listen :
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "Listening failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Listening on port : " <<port<<endl;
	vector<SOCKET> clients;
    while (1) {
        //accept :
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);

        if (clientSocket == INVALID_SOCKET)
        {
            cout << "Accepting failed" << endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
		clients.push_back(clientSocket);
        thread t1(InteractWithClient, clientSocket,std::ref(clients));
        t1.detach();
    }

	closesocket(listenSocket);
    WSACleanup();
    return 0;
}
