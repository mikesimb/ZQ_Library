// ------------------------------------------------------------------------- 
//	文件名		: 	TcpClientSocket.h
//	创建者		:	胡小帆
//	创建时间	:	
//	功能描述	:	客户端通讯组件
//
//  详细描述    :
// -------------------------------------------------------------------------
#ifndef __TCPCLIENTSOCKET_H_
#define __TCPCLIENTSOCKET_H_

#include <winsock2.h>
#include <string>

using namespace std;

class CClientSocketCallBackInterface
{
public:
	virtual void OnRead(const char *pBuf, INT nCount) {};
	virtual void OnError(INT &ErrorCode) {};
	virtual void OnConnected(INT nBackFlags) {};
	virtual void OnDisconnect(INT nBackFlags) {};
};

typedef struct tagSendBufferNode
{
	char *szBuf;
	INT nBufLen;                                       // 长度
	INT nStart;                                        // 起始位置
	tagSendBufferNode *pNext;
}SendBufferNode;

typedef enum SOCKET_ERROR_TYPE
{
	seConnect	=0,
	seRead		=1, 
	seSend		=2, 
	seClose		=3
};

class CTcpClientSocket
{
public:
	CTcpClientSocket(CClientSocketCallBackInterface *pCallBackIntf = NULL);
	virtual ~CTcpClientSocket();

public:
	void SetCallback(CClientSocketCallBackInterface *pCallBackIntf);
	DWORD SendBuf(const char *pBuf, DWORD Len, BOOL nAsync = FALSE);
	BOOL Open(const string &Addr, WORD wPort);
	BOOL Close();
	virtual BOOL Execute();                              // Single?

	BOOL IsActive() const { return m_bActive; }
	BOOL IsConnected() const { return m_bConnected; }

	void SetBackFlags(INT Value) { m_nBackFlags = Value; }
	INT GetBackFlags() const { return m_nBackFlags; }

protected:
	virtual void OnRead(char *pBuf, INT nCount);
	virtual void OnConnected();
	virtual void OnDisconnet();
	virtual void OnError(INT &ErrorCode);

protected:
	RTL_CRITICAL_SECTION m_SendCS;
	SendBufferNode *m_First;
	SendBufferNode *m_Last;                       // 发送缓冲区链表
	INT m_Count;                                  // 发送队列的个数

	void DoRead();
	void DoWrite();
	BOOL DoError(SOCKET_ERROR_TYPE seType);
	BOOL DoInitialize();
	void DoSend();
	


private:
	SOCKET m_CSocket;
	WSAEVENT m_Event;
	string m_Address;
	WORD m_Port;
	BOOL m_bActive;
	BOOL m_bConnected;

	INT m_nBackFlags;

	CClientSocketCallBackInterface *m_pCallBackIntf;
};

#endif //#ifndef