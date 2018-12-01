#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024
#define YANG

#pragma comment(lib,"Ws2_32.lib ")
int main()
{
	WSADATA wsaData;
	struct sockaddr_in serAddr;
	SOCKET clinSock;
	char buf[BUF_SIZE];
	char pathBuf[BUF_SIZE];
	int msgLen=0;
	int msgNum=0;
	int i=0;
	int recvMsgLen=0;
	int result=0;
	int j=0;

	if (WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	{
		printf("WSAStartup() error\n");
		exit(1);
	}

	clinSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(INVALID_SOCKET == clinSock)
	{
		printf("socket() error\n");
		exit(1);
	}

	result=1;
	if(SOCKET_ERROR == setsockopt(clinSock,IPPROTO_TCP,TCP_NODELAY,&result,sizeof(result)))
	{
		printf("setsockopt() error\n");
		exit(1);
	}

	memset(&serAddr,0,sizeof(serAddr));
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(3333);
	serAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(clinSock,(const struct sockaddr*)&serAddr,sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect() error\n");
		exit(1);
	}

	printf("connected to server!\n");

	while (j<100)
	{
#if 1	
		printf("please input send msg:");
		scanf("%s",pathBuf);
		if (strcmp(pathBuf,"q") == 0)
		{
			break;
		}
#endif
		msgLen=0;
		msgLen = strlen(pathBuf);
		send(clinSock,pathBuf,msgLen,0);

		msgLen=0;
		recv(clinSock,(char*)&msgLen,sizeof(int),0);
		recv(clinSock,(char*)&pathBuf,msgLen,0);
		pathBuf[msgLen] = '\0';
		printf("msg: %s,msg length is %d\n",pathBuf,msgLen);

#if 0
		for (i=0;i<msgNum;i++)
		{
			recv(clinSock,(char*)&msgLen,4,0);
			recvMsgLen=0;
			while (recvMsgLen<msgLen)
			{
				recvMsgLen+= recv(clinSock,(char*)&buf[recvMsgLen],msgLen-recvMsgLen,0);
			}
		}
#endif
		j++;
	}
	
	closesocket(clinSock);   //closesocket和WSACleanup的顺序不能反
	WSACleanup();
	return 0;
}