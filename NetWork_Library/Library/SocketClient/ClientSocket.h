#ifndef __CLIENTSOCKET_H_
#define __CLIENTSOCKET_H_

#include "TcpClientSocket.h"
#include <string>
#include <map>
#include "../Utils/StreamBuffer.h"
#include "../Client/Buffer.h"

#define PACKET_BUF_LEN			65536 // 64k

using namespace std;

/*
(Head)											   (Data)
/-------------------------------------------------------------------\    /-------------\
|网络包开始标识(4字节)|包的总长度(2字节)|协议号(2字节)|自增的包序号(4字节)|   |	没压缩的数据	|


如果Ident>=0x8000 表示此封包为压缩数据，需要去掉高位得到正确的Ident				

(Head)													  (Data)
/-------------------------------------------------------------------\   /----------------------------\
|网络包开始标识(4字节)|包的总长度(2字节)|协议号(2字节)|自增的包序号(4字节)|   | 原始长度(2字节) | 压缩的数据 |

*/

#pragma pack(1)
typedef struct tagHead
{
	DWORD	dwStartFlag;	// 网络包开始标识 0xAABBCCDD
	WORD	wPackageLen;	// 包的总长度(包括包头)
	WORD	wIdent;			// 协议ID(Ident>=0x8000表示，数据部分被压缩)
	DWORD	dwAutoID;		// 自增长ID
}Head, *PHead;

typedef struct tagMemberFuncInfo
{
	DWORD dwThis;
	DWORD dwFuncAddr;
}MemberFuncInfo;
#pragma pack()

#define SOCKETPACKET_START_SYMBOL	0xAABBCCDD
const WORD COMPRESS_FLAG = 0x8000;

class CSendPacketData
{
public:
	CSendPacketData(){}
	virtual ~CSendPacketData(){}

	char* GetHeadBuf()
	{
		return (char*)m_szBuf;
	}

	int GetHeadBufLen()
	{
		return sizeof(Head);
	}

	char * GetDataBuf()
	{
		return (char*)((BYTE*)m_szBuf + sizeof(Head));
	}

	char * GetComDataBuf()
	{
		return (char*)((BYTE*)m_szBuf + sizeof(Head) + sizeof(WORD)); // sizeof(WORD)为放置原始数据长度大小的空间
	}

	int GetDataBufLen()
	{
		return PACKET_BUF_LEN - sizeof(Head);
	}

	char* GetBuf()
	{
		return (char*)m_szBuf;
	}

	int GetBufLen()
	{
		return sizeof(m_szBuf);
	}

private:
	BYTE* m_szBuf[PACKET_BUF_LEN];
};

typedef  void (__stdcall *FUNCTYPE)(char* pBuf, int nLen);
typedef  void (__stdcall *FUNCDBG)(WORD wID, int nLen);

//////////////////////////////////////////////////////////////////////////
//CSocketBuffer
class CSocketBuffer
{
public:
	CSocketBuffer();
	~CSocketBuffer();

public:
	BOOL Append(CONST CHAR *pData, INT nSize);
	virtual BOOL Get(CHAR **ppData, INT &nSize);
	void Clear();

protected:
	void SetCapacity(INT nSize);

protected:
	CHAR *m_pData;
	INT m_nReadPos;
	INT m_nWritePos;
	INT m_nCapacity;

};

//加解密函数
typedef void (__cdecl * TCodingProc)(BYTE *pInBuf, WORD wLen);

//////////////////////////////////////////////////////////////////////////
//CGPCheckThread

#ifdef _GPROTECT
class CGPCheckThread
{
public:
	CGPCheckThread();
	~CGPCheckThread();

public:
	void SetCheckData(CHAR* pBuf, INT nLen);

protected:
	static void agent(void *p);
	void	ThreadExecute();

private:
	RTL_CRITICAL_SECTION m_csLock;
	BOOL		m_Terminated;
	HANDLE		m_hThread;

	char*		m_pBuf;
	int			m_nLen;

};
#endif //_GPROTECT

//////////////////////////////////////////////////////////////////////////
//CClientSocket
class CClientSocket : public CTcpClientSocket
{
public:
	typedef map< WORD, MemberFuncInfo > SocketFuncMap;

public:
	CClientSocket(CClientSocketCallBackInterface *pCallBackIntf = NULL);
	virtual ~CClientSocket();

	virtual BOOL Execute();  
	BOOL	SendData(WORD wID, LPCSTR pData = NULL, int nDataLen = 0);
	BOOL	RegFunc(WORD wID, const MemberFuncInfo& info);

	void    SetDbgCB(MemberFuncInfo& info);

protected:
	virtual void OnRead(char *pBuf, INT nCount);
	virtual void OnDisconnet();

private:
	void	ProcessOneSocketpacket(WORD wIdent, char* pBuf, int nLen);
	BOOL	Dispatch(WORD wID, char* pBuf, int nLen);

private:
	SocketFuncMap	m_SocketFuncMap;
	CSendPacketData	m_SendData;

	CSocketBuffer	m_RecvBuffer;
	
	TCodingProc		m_pEncode;
	TCodingProc		m_pDecode;

	DWORD           m_pFnPtr;
	DWORD           m_dwThis;

	RTL_CRITICAL_SECTION m_csLock;

	#ifdef _GPROTECT
	CGPCheckThread	m_GPCheckThread;
	#endif //_GPROTECT

	//DWORD			m_dwLastSendTick;
	//DWORD			m_dwLastSendTick2;
};

__declspec(selectany) CClientSocket g_socket;

//////////////////////////////////////////////////////////////////////////
template<class ToType, class FromType>
inline void GetMemberFuncAddr(ToType& addr, FromType f)
{
	union 
	{
		FromType _f;
		ToType   _t;
	}ut;

	ut._f = f;
	addr = ut._t;
}


#define RegSocketFuncInit							\
	MemberFuncInfo info

/*
用于注册 成员函数的Socket回调
ID			: 协议ID
FuncAddr	：成员函数地址
注意			：成员函数只能是__thiscall类型
*/
#define RegSocketFunc(ID, MemberFuncAddr)			\
	info.dwThis = (DWORD)this;							\
	GetMemberFuncAddr(info.dwFuncAddr, MemberFuncAddr);	\
	g_socket.RegFunc(ID, info);

#define RegSocketDbgFunc(MemberFuncAddr) \
	info.dwThis = (DWORD)this; \
	GetMemberFuncAddr(info.dwFuncAddr, MemberFuncAddr); \
	g_socket.SetDbgCB(info);

#endif //__CLIENTSOCKET_H_