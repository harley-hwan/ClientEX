#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100
#define NAME_SIZE 30

unsigned WINAPI SendMsg(void* arg);
unsigned WINAPI RecvMsg(void* arg);
void ErrorHandling(const char* msg);
int ip = 0;

char msg[BUF_SIZE];

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET sock;
    SOCKADDR_IN serverAddr;
    HANDLE sendThread, recvThread;
   
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        ErrorHandling("WSAStartup() error!");
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // ���� IP ����
    serverAddr.sin_port = htons(2483); // ���� Port ����

    // �ۼ����� ���� �������� serverAddr�� ���� ������ �ѱ�.
    if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error");

    // �޴� ������ ����
    recvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&sock, 0, NULL);

    strcpy(msg, "{\"Type\":\"Handshake\"}");
    send(sock, msg, strlen(msg), 0);

    while (1)
    {
        fgets(msg, BUF_SIZE, stdin);  // abc\n\0

        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        { // q�Է½� ����
            closesocket(sock);
            exit(0);
        }
        msg[strlen(msg) - 1] = '\0';
        send(sock, msg, strlen(msg), 0);
    }
    
    closesocket(sock); // ���� ����
    WSACleanup(); // �ڿ� �Ҵ� ����
    return 0;
}


// �޴� �������� ����
unsigned WINAPI RecvMsg(void* arg)
{
    SOCKET sock = *((SOCKET*)arg);
    char nameMsg[NAME_SIZE + BUF_SIZE];
    int strLen;
    while (1)
    {
        strLen = recv(sock, nameMsg, NAME_SIZE + BUF_SIZE - 1, 0);
        if (strLen == -1)
            return -1;
        nameMsg[strLen] = 0;
        printf("received %s\n", nameMsg);
    }
    return 0;
}

void ErrorHandling(const char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
