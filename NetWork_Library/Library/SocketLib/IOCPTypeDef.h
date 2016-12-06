#ifndef  _CIOCPTypeDef_H__
#define  _CIOCPTypeDef_H__


//链接静态的SOKCET库
#pragma comment(lib, "ws2_32.Lib")


#include <WinSock2.h>
#include <stdint.h>
#include <functional>
#include <process.h>
#include "ZQString.h"



bool WSAStartupSocket();
bool WSACleanupSocket();

int inet_ntop4(const unsigned char *src, char *dst, size_t size);
CZQString intTostr(int Value);
int strToint(CZQString str);

#define MAX_CLIENT_COUNT		( 1000)
#define MAX_IOCP_BUF_SIZE		( 8 * 1024)
#define SHUTDOWN_FLAG			( 0xFFFFFFFF)
#define DISCONNECT_FLAG			( 0xFFFFFFFE)

#define MAX_WORK_THREAD_SIZE		( 16)
#define HASH_IPITEM_BUCKETS		( 2047 )
#define MAX_CHECK_TIMEOUT		( 30 * 1000)
#define MAX_ACTIVE_TIMEOUT		( 10 * 60)
#define MAX_FREE_TIMEOUT		( 3  * 60)
#define CLIENT_MAX_RECEIVE_SIZE ( 32 * 1024 )
#define SERVER_MAX_SEND_SIZE    ( 64 * 1024 )
#define MSG_SIGN                ( 0xFF44FF44)
#define SERVER_MSG_SIGN         ( 0xFF33FF33)


//事件定义类型
enum  SocketEvent {
	ZQ_seInitIOPort = 0,
	ZQ_seInitSocekt,
	ZQ_seConnect,
	ZQ_seListen,
	ZQ_seAccept,
	ZQ_seWrite,
	ZQ_seRead
};

class CZQ_CustomIOCPClientSocket;
class CZQ_CustomIOCPSocket;
class CZQ_IOCPClientSocket;
class CZQ_CustomIOCPServerSocket;
class CZQ_WorkThread;
class CZQ_AcceptThread;
class CZQ_CustomClientSocketPool;
class CZQ_ClientIOCPSocket;

typedef struct tagIOCPSOCKET {
	SOCKET socket;
	char   IP[16];
	uint16_t  port;
}IOCPSOCKET,*pIOCPSOCKET;

//完成端口缓冲区结构
typedef struct _Block
{
	OVERLAPPED overlapped;
	WSABUF  wsaBuffer;
	SocketEvent  event;
	char Buffer[MAX_IOCP_BUF_SIZE];

}Block,*pBlock;

//发送队列节点
typedef struct _SendQueueNode
{
	char * szBuf;
	int iBufferLen;
	int iStartPosition;
	_SendQueueNode* next;
}SendQueueNode,*pSendQueueNode;
//这个队列是
typedef struct _HASH_PORT_ITEM
{
	uint32_t SocketHandle;
	CZQ_CustomIOCPClientSocket * CustomSocket;
	_HASH_PORT_ITEM* next;

}HashPortItem,*pHashPortItem;

typedef struct _CustomClientListSocketNode
{
	CZQ_CustomIOCPClientSocket* clientSocket;
	_CustomClientListSocketNode * next;
}CustomClientListSocketNode,*pCustomClientListSocketNode;


//基础的消息类型
typedef struct _Ref
{

}Ref;

struct MsgRef :public Ref
{
	uint32_t  sign;
	uint16_t  datalength;
	uint16_t  cmd;
	uint32_t  Reserved;
	MsgRef()
	{
		sign = 0;
		datalength = 0;
		cmd = 0;
	}
};

struct MsgHead :public MsgRef
{
	MsgHead() {
		sign = MSG_SIGN;
		datalength = sizeof(*this);

	}
};

struct ServerMsgHead :public MsgRef {
	int iSocketHandle;
	ServerMsgHead()
	{
		sign = SERVER_MSG_SIGN;
		datalength = sizeof(*this);
		iSocketHandle = 0;
	}
};



//回调函数定义
#define  SocketNotifyEvent std::function<void(CZQ_CustomIOCPClientSocket*)>
#define  SocketErrorEvent std::function<void(CZQ_CustomIOCPClientSocket* ,int&)>
#define	 ClientSocketTouchProc std::function<void(CZQ_CustomIOCPClientSocket* ,uint32_t)>
#define  SocketReviceEvent   std::function<void(CZQ_CustomIOCPClientSocket* ,void* ,int)>
#define  OnCheckValidEvent  std::function<bool(CZQString)>

#define OnReviceEvent std::function<void(void*,int)>
#define OnSocketErrorEvent std::function<void(CZQ_CustomIOCPClientSocket* ,int)>
#define OnAcceptEvent std::function<void(SOCKET , char* ,int)>

#define CreateCustomSocketEvent  std::function<CZQ_CustomIOCPClientSocket*()>
#define OnProcessMsgEvent   std::function<void(MsgRef*,int);
#define OnProcessServerMsgEvent  std::function<void(ServerMsgHead*,int)>

#define ARRAY_SIZE(a)  (sizeof(a)/sizeof(a[0]))

//C++11 回调的定义
#define IOCP_EVENT_CALLBACK_0(__selector__, __target__, ...)  std::bind(&__selector__, __target__, ##__VA_ARGS__)
#define IOCP_EVENT_CALLBACK_1(__selector__, __target__, ...)  std::bind(&__selector__, __target__, std::placeholders::_1, ##__VA_ARGS__) 
#define IOCP_EVENT_CALLBACK_2(__selector__, __target__, ...)  std::bind(&__selector__, __target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define IOCP_EVENT_CALLBACK_3(__selector__, __target__, ...)  std::bind(&__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)








#endif

