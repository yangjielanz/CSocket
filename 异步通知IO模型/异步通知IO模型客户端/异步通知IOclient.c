#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 1024
char buf[BUF_SIZE];

void errorHanding(char *errMsg)
{
	printf("%s\n",errMsg);
	getchar();
	exit(1);
}

DWORD WINAPI readFun(LPVOID p)
{
	SOCKET clinSocket = *(SOCKET*)p;
	int msgLen=0;
	char buf[BUF_SIZE];

	while (1)
	{
		msgLen = recv(clinSocket,buf,BUF_SIZE,0);
		if ((0==msgLen)||(SOCKET_ERROR == msgLen))
		{
			//shutdown(clinSocket,2);
			closesocket(clinSocket);
			break;
		}
		printf("\n");
		buf[msgLen] = '\0';
		printf("client recv msg %s\n",buf);
		
	}

	return 0;
}


DWORD WINAPI sendFun(LPVOID p)
{
	SOCKET clinSocket = *(SOCKET*)p;
	int msgLen=0;
	char buf[BUF_SIZE];

	while (1)
	{
		printf("input msg:");

		scanf("%s",buf);
		if ((strcmp(buf,"q")==0)||(strcmp(buf,"Q")==0))
		{
			//shutdown(clinSocket,1);
			closesocket(clinSocket);
			break;
		}

		msgLen = send(clinSocket,buf,strlen(buf),0);
	}

	return 0;
}


int main()
{
	SOCKET clinSocket;
	struct sockaddr_in serAddr;
	int len=0;
	WSADATA wsaData;
	HANDLE THREADh[2] = {NULL};


	if (WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	{
		errorHanding("WSAStartup() ERROR!");
	}

	clinSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == clinSocket)
	{
		errorHanding("socket() ERROR!");
	}

	memset(&serAddr,0,sizeof(serAddr));
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8888);
	serAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (SOCKET_ERROR == connect(clinSocket,(struct sockaddr*)&serAddr,sizeof(serAddr)))
	{
		errorHanding("socket() ERROR!");
	}
	THREADh[0] = CreateThread(NULL,0,sendFun,&clinSocket,0,NULL);
	THREADh[1] = CreateThread(NULL,0,readFun,&clinSocket,0,NULL);

	WaitForMultipleObjects(2,THREADh,TRUE,INFINITE);

	closesocket(clinSocket);
	WSACleanup();
	return 0;
}