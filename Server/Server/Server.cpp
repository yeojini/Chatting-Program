#include <iostream>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")

#define PORT 8888
#define IP "127.0.0.1"

HANDLE hSendThread,hReceiveThread;

int server_socket = 0;
int client_socket = 0;
int client_size = 0;
int client_number = 0;
int client_sockets[10];


void Sender_thread(void *p) {
	while (true) {
		char buff[1024] = { 0 };
		printf(">>");
		std::cin >> buff;
		if (strlen(buff) == 0) {
			break;
		}
		else {
			for (int i = 0; i < client_number; i++) {
				send(client_sockets[i], buff, strlen(buff), 0);
			}
		}
	}
}

void Receive_thread(void *p) {
	SOCKET sock = (SOCKET)p;
	char buff[1024] = {0.};
	while (true) {

		//수신
		int recvsize = recv(client_socket, buff, sizeof(buff), 0);
		if (recvsize<0) {
			printf("접속 종료\n");
			break;
		}
		printf("\nfrom Client : %s\n",buff);
		memset(buff, 0, 1024);
	}
	closesocket(sock);
}

int main() {

	std::cout << "Welcome!" << std::endl;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	char buff[024];
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		printf("Winsock 오류\n");
		return 1;
	}


	//소켓 생성
	SOCKADDR_IN server = { 0 };

	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (server_socket == SOCKET_ERROR) {
		printf("socket 생성 오류\n");
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	//소켓 구성요소 담을 구조체 생성, 값할당. 포트 구성
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	//server socket handle 에 server address 정보 bind
	if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("연결 오류 \n");
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	else printf("연결 성공\n");

	//server socket handle 연결 대기 상태로 변경
	//대기 인원은 5명까지
	if (listen(server_socket, 5) == -1) {
		printf("Listen 오류 \n");
	}

	SOCKADDR_IN client = { 0 };
	
	while (true) {

		client_size = sizeof(client);
		client_socket = accept(server_socket, (sockaddr*)&client, &client_size);
		if (client_socket == SOCKET_ERROR) {
			printf("Accept 오류 \n");
			return 1;
		}
		
		client_sockets[client_number++] = client_socket;
		printf("클라이언트 접속, IP : %s \n", inet_ntoa(client.sin_addr));
		
		hSendThread = (HANDLE)_beginthread(Receive_thread, 0, (void*)client_socket);
		hReceiveThread=(HANDLE)_beginthread(Sender_thread, 0, (void*)client_sockets);
	
		WaitForSingleObject(hSendThread, INFINITE);
		WaitForSingleObject(hReceiveThread, INFINITE);
	
	}

	CloseHandle(hSendThread);
	CloseHandle(hReceiveThread);

	closesocket(client_socket);
	closesocket(server_socket);
	WSACleanup();

	return 0;

}

