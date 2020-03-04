#include <iostream>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <string>
#include <thread>
#include <mutex>
#include <process.h>
#pragma comment(lib,"ws2_32.lib")

#define PORT 8888
#define IP "127.0.0.1"

int client_socket = 0;

void Receive_thread(void *p) {
	SOCKET sock = (SOCKET)p;
	char buff[1024] = { 0. };
	while (true) {

		//수신
		int recvsize = recv(client_socket, buff, sizeof(buff), 0);
		if (recvsize < 0) {
			printf("접속 종료\n");
			break;
		}
		printf("\nfrom Server : %s\n", buff);
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

	SOCKADDR_IN client = { 0 };
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket == SOCKET_ERROR) {
		printf("socket 생성 오류\n");
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}
	//소켓 구성요소 담을 구조체 생성, 값할당. 포트 구성
	client.sin_family = AF_INET;
	client.sin_port = htons(PORT);
	client.sin_addr.s_addr = inet_addr("127.0.0.1");

	//connet 함수를 통해 listen 상태의 서버에서 접속 요청 시도
	if (connect(client_socket, (SOCKADDR*)&client, sizeof(client)) == SOCKET_ERROR) {
		printf("Connect() Error\n");
			return 1;
	}
	
	printf("서버와 연결되었습니다. \n");

	while (true) {
		
		char buff[1024];
		_beginthread(Receive_thread, 0, (void*)client_socket);

		printf(">>");
		scanf("%s", &buff);

		if (sizeof(buff) == 0) {
			break;
		}
		
		else {
			send(client_socket, buff, strlen(buff), 0);
			memset(buff, 0, 1024);
		}

	}

	closesocket(client_socket);
	WSACleanup();
	

}