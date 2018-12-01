#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib")


void errorHandle(char *msg)
{
	fputs(msg,stdin);
	fputc('\n',stdin);
	getchar();
	exit(1);
}

int main()
{
	struct sockaddr_in serAddr,cliAddr;
	SOCKET serSocket,cliSocket; 
	int sddrLen=0;
	int msgLen=0;
	fd_set reads={0},copyRead={0},except={0},copyExcept={0};
	WSADATA wsaData;
	WORD word = MAKEWORD(2,2);
	TIMEVAL timeout;
	int result = SOCKET_ERROR;
	unsigned int i=0;
	char buf[1024];
	int state;
	int temp;

	if (WSAStartup(word,&wsaData)!=0)
	{
		errorHandle("WSAStartup() error!");
	}

	serSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == serSocket)
	{
		errorHandle("socket() error!");
	}
	state=1;
	temp = setsockopt(serSocket,SOL_SOCKET,SO_REUSEADDR,&state,sizeof(int));
	if (-1 == temp)
	{
		errorHandle("setsockopt() error!");
	}
	memset(&serAddr,0,sizeof(serAddr));
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(3333);
	serAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (SOCKET_ERROR == bind(serSocket,(struct sockaddr*)&serAddr,sizeof(serAddr)))
	{
		errorHandle("bind() error!");
	}

	if (SOCKET_ERROR == listen(serSocket,5))
	{
		errorHandle("listen() error!");
	}

	FD_ZERO(&reads);
	FD_SET(serSocket,&reads);
	FD_ZERO(&except);

	while(1)
	{
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		copyRead = reads;
		copyExcept = except;
		result = select(0,&copyRead,0,&copyExcept,&timeout);
		if (SOCKET_ERROR == result)
		{
			errorHandle("select() error!");
			break;
		}
		else if (0 == result)
		{
			printf("timeout!\n");
			continue;
		}
		else
		{
			for (i=0;i<except.fd_count;i++);
			{
				if (FD_ISSET(copyExcept.fd_array[i],&copyExcept))
				{
					msgLen = recv(copyExcept.fd_array[i],buf,1024,MSG_OOB);
					buf[msgLen] = '\0';
					printf("out of band data is %s\n",buf);
				}
			}

			for (i=0;i<reads.fd_count;i++)
			{
				if (FD_ISSET(copyRead.fd_array[i],&copyRead))
				{
					if (copyRead.fd_array[i] == serSocket)
					{
						sddrLen = sizeof(struct sockaddr_in);
						cliSocket = accept(serSocket,(struct sockaddr*)&cliAddr,&sddrLen);
						FD_SET(cliSocket,&reads);
						FD_SET(cliSocket,&except);
						printf("connect new client %d,%d\n",cliSocket,reads.fd_count);
					}
					else
					{
						msgLen = recv(copyRead.fd_array[i],buf,1024,0);
						if ((0 == msgLen)||(0 > msgLen))
						{
							closesocket(copyRead.fd_array[i]);
							FD_CLR(copyRead.fd_array[i],&reads);
							printf("a client disconnect %d\n",copyRead.fd_array[i]);
						}
						else
						{
							buf[msgLen] = '\0';
							printf("recv msg %s from %d\n",buf,copyRead.fd_array[i]);
							send(copyRead.fd_array[i],(char *)&msgLen,4,0);
							send(copyRead.fd_array[i],buf,msgLen,0);
						}
					}
				}
			}

			
		}
	}
	closesocket(serSocket);
	WSACleanup();
	return 0;
}