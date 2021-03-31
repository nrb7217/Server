#define WIN32_LEAN_AND_MEAN

#include<windows.h>
#include<winsock2.h>
#include<stdio.h>
#include<WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFFLEN 512
#define DEFAULT_PORT "27015"


DWORD WINAPI handleClient(SOCKET clientSocket) {
	char messageRecv[DEFAULT_BUFFLEN];
	char endmessage[DEFAULT_BUFFLEN] = "quit";
	int numMessages = rand() % (10 + 1 - 2) + 2;
	int i = 0;
	int result = 0;
	result = recv(clientSocket, messageRecv, DEFAULT_BUFFLEN, 0);
	if (result == SOCKET_ERROR) {
		printf("Failed to receive message from client\n");
		//WSACleanup();
		return 1;
	}
	if (strcmp(messageRecv, endmessage) == 0) {
		shutdown(clientSocket, SD_SEND);
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}
	else {
		printf("Received %s\n", messageRecv);
		//send
		while (i < numMessages || i == numMessages) {
			result = send(clientSocket, messageRecv, DEFAULT_BUFFLEN, 0);
			if (result == SOCKET_ERROR) {
				printf("Failed to send message to client\n");
				//WSACleanup();
				return 1;
			}
			if (i == numMessages) {
				send(clientSocket, "**DONE**", DEFAULT_BUFFLEN, 0);
			}
			i++;
		}
		handleClient(clientSocket);
	}

}

int main() {
	WSADATA wsaData;
	int iResult = 0;
	SOCKET serverSocket = INVALID_SOCKET;
	//initialize use of WS2_32.dll
	//HANDLE hThread;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSA startup failed with error code %d\n", iResult);
		WSACleanup();
		return 1;
	}

	//getaddrinfo for result

	struct addrinfo* result = NULL;
	struct addrinfo hints;
	

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// create serversocket
	serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (serverSocket == INVALID_SOCKET) {
		printf("socket failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	//bind socket to address
	iResult = bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error code %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);

	// listen
	listen(serverSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listening failed with error %d\n", WSAGetLastError());
		serverSocket = INVALID_SOCKET;
		//WSACleanup();
		return 1;
	}
	printf("Server is listening. \n");
	//accept
	SOCKET clientSocket = accept(serverSocket, NULL, NULL);
	if (iResult == SOCKET_ERROR) {
		printf("connection failed with error %d\n", WSAGetLastError());
		//WSACleanup();
		return 1;
	}
	printf("Connecting Client\n");
	send(clientSocket, "Thank you for connecting!", DEFAULT_BUFFLEN, 0);
	//print welcome message when client connects displaying client ID
	
	/*
	for (int i = 0; i <= 10; i++) {
		HANDLE hThread = CreateThread(NULL, 0, handleClient, &clientSocket, 0, NULL);

		if (hThread == NULL) {
			printf("create thread failed with error code: %d\n", GetLastError());
			return 1;
		}
		
	}
	*/
	
	
	handleClient(clientSocket);

	
	//close connection

	shutdown(clientSocket, SD_SEND);
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}