#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 1024
void errorHandle(char *msg)
{
	fputs(msg,stdin);
	fputc('\n',stdin);
	getchar();
	exit(1);
}


int main()
{
	struct sockaddr_in serAddr;
	SOCKET cliSock;
	char buf[BUF_SIZE];
	WSADATA wsaData;
	int state,result;

	if (WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	{
		errorHandle("WSAStartup() error!");
	}

	cliSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(INVALID_SOCKET == cliSock)
	{
		errorHandle("socket() error!");
	}

	result=1;
	state = setsockopt(cliSock,IPPROTO_TCP,TCP_NODELAY,&result,sizeof(result));
	if (SOCKET_ERROR == state)
	{
		errorHandle("setsockopt() error!");
	}

	memset(&serAddr,0,sizeof(serAddr));
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(3333);
	serAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (SOCKET_ERROR == connect(cliSock,(struct sockaddr*)&serAddr,sizeof(serAddr)))
	{
		errorHandle("connect() error!");
	}

	send(cliSock,"yangjie",strlen("yangjie"),0);
	send(cliSock,"zhao",4,MSG_OOB);
	send(cliSock,"NIHAO",strlen("NIHAO"),0);
	send(cliSock,"DEF",3,MSG_OOB);

	getchar();
	closesocket(cliSock);
	WSACleanup();
	return 0;
}