#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")
#define BUF_SIZE 1024
#define MAX_SOCKET_NUM 100

SOCKET socketArray[MAX_SOCKET_NUM] = {INVALID_SOCKET};
WSAEVENT eventArray[MAX_SOCKET_NUM] = {NULL};
char msgBuf[BUF_SIZE];

void processSocketArray(SOCKET socketArray[],int index,int totleNum)
{
	int i=0;
	for (i=index;i<totleNum;i++)
	{
		socketArray[i] = socketArray[++i];
	}
}

void processEventArray(WSAEVENT eventArray[],int index,int totleNum)
{
	int i=0;
	for (i=index;i<totleNum;i++)
	{
		eventArray[i] = eventArray[++i];
	}
}

void errorHanding(char *errMsg)
{
	printf("%s\n",errMsg);
	getchar();
	exit(1);
}


int main()
{
	SOCKET serSock,clinSock;
	struct sockaddr_in serAddr,clinAddr;
	int clinAddrLen=0;
	WSAEVENT newEvent;
	WSADATA wsaData;
	int num=0;
	int msgLen=0;
	DWORD posion;
	DWORD index=0,currentIndex=0;
	int i=0;
	int state;
	int opetion=1;

	WSANETWORKEVENTS eventInf;


	if (WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	{
		errorHanding("WSAStartup() error!");
	}

	if (LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2)
	{
		errorHanding("1WSAStartup() error!");
	}

	serSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == serSock)
	{
		errorHanding("socket() error!");
	}
	setsockopt(serSock,SOL_SOCKET,SO_REUSEADDR,(char *)&opetion,sizeof(opetion));

	memset(&serAddr,0,sizeof(serAddr));
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8888);
	serAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (SOCKET_ERROR == bind(serSock,(struct sockaddr *)&serAddr,sizeof(serAddr)))
	{
		errorHanding("bind() error!");
	}

	if (SOCKET_ERROR == listen(serSock,5))
	{
		errorHanding("listen() error!");
	}

	newEvent = WSACreateEvent();
	if (WSA_INVALID_EVENT == newEvent)
	{
		errorHanding("WSACreateEvent() error!");
	}
	if (SOCKET_ERROR == WSAEventSelect(serSock,newEvent,FD_ACCEPT))
	{
		errorHanding("WSAEventSelect() error!");
	}

	socketArray[num] = serSock;
	eventArray[num] = newEvent;
	num++;

	while (1)
	{
		posion = WSAWaitForMultipleEvents(num,eventArray,FALSE,WSA_INFINITE,FALSE);
		printf("11111   %d\n",num);
		index = posion - WSA_WAIT_EVENT_0;
		for (i=index;i<num;i++)
		{
			printf("2222   %d\n",num);
			currentIndex = WSAWaitForMultipleEvents(1,&eventArray[i],TRUE,0,FALSE);
			if ((currentIndex == WSA_WAIT_FAILED)||(currentIndex == WSA_WAIT_TIMEOUT))
			{
				continue;
			}
			state = WSAEnumNetworkEvents(socketArray[i],eventArray[i],&eventInf);
			if (SOCKET_ERROR == state)
			{
				continue;
			}

			if (eventInf.lNetworkEvents&FD_ACCEPT)
			{
				if (eventInf.iErrorCode[FD_ACCEPT_BIT]!=0)
				{
					printf("accept error!\n");
					break;
				}

				printf("new client connect!\n");
				clinAddrLen = sizeof(clinAddr);
				clinSock = accept(socketArray[i],(struct sockaddr*)&clinAddr,&clinAddrLen);
				if (INVALID_SOCKET == clinSock)
				{
					printf("accept error!\n");
					break;
				}

				newEvent = WSACreateEvent();
				if (WSA_INVALID_EVENT == newEvent)
				{
					errorHanding("WSACreateEvent() error!");
				}
				if (SOCKET_ERROR == WSAEventSelect(clinSock,newEvent,FD_READ|FD_CLOSE))
				{
					errorHanding("WSACreateEvent() error!");
				}
				socketArray[num]=clinSock;
				eventArray[num]=newEvent;
				num++;
				continue;
			}

			if (eventInf.lNetworkEvents&FD_READ)
			{
				if (eventInf.iErrorCode[FD_READ_BIT]!=0)
				{
					printf("read error!\n");
					break;
				}
				msgLen = recv(socketArray[i],msgBuf,BUF_SIZE,0);
				msgBuf[msgLen] = '\0';
				send(socketArray[i],msgBuf,msgLen,0);
				continue;
			}
		

			if (eventInf.lNetworkEvents&FD_CLOSE)
			{
				/*if (eventInf.iErrorCode[FD_CLOSE_BIT]!=0)
				{
					printf("close error!\n");
					break;
				}*/
				//shutdown(socketArray[i],1);
				//
				closesocket(socketArray[i]);
				WSACloseEvent(eventArray[i]);

				num--;
				processSocketArray(socketArray,i,num);
				processEventArray(eventArray,i,num);
				continue;
			}
		}
	}

	WSACleanup();
	closesocket(serSock);
	return 0;
}