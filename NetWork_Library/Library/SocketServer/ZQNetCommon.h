#pragma once
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")

#define MAX_BUFFER_LEN     1024
#define DISCONNECT_FLAG   0xFFFFFFFE
#define SHUTDOWN_FLAG    0xFFFFFFFF

typedef enum { ioNone, ioRead, ioWrite }  IoOperator;

typedef struct  __Blocktag
{
	
	WSAOVERLAPPED  overloapped;
	WSABUF     buf;
	char     MsgBuf[MAX_BUFFER_LEN];
	IoOperator OperatorType;
	
}Block, *pBlock;


typedef struct __ClientSendDataNodetag
{
	char * Buf;
	int       buflen;
	int       iStartPosition;
	__ClientSendDataNodetag * Next;

}ClientSendDataNode, *pClientSendData;

