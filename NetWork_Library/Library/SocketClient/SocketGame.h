// ------------------------------------------------------------------------- 
//	文件名	: 	SocketGame.h
//	创建者	:	方煜宽
//	创建时间:	2011-12-8 16:13
//	功能描述:	游戏客户端Socket
//  注意	: 	包含本头文件(GameSocket.h)请在<windows.h>前包含
//
// -------------------------------------------------------------------------
#ifndef __SocketGame_h__
#define __SocketGame_h__

#include "Socket.h"
#include <string>
#include <map>
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

class CSocketGame : public CSocket
{
public:
	typedef map< WORD, MemberFuncInfo > SocketFuncMap;
	
public:
	CSocketGame();
	virtual ~CSocketGame();

	BOOL	SendData(WORD wID, LPCSTR pData = NULL, int nDataLen = 0);
	void	ProcessSocketpacket();											// 游戏循环里调用
	BOOL	RegFunc(WORD wID, const MemberFuncInfo& info);

protected:
	virtual BOOL	CheckSocketError(LPARAM lParam);
	virtual void	OnConnect(SOCKET host);
	virtual void	OnClose(SOCKET host);

private:
	int		FindStarPos(char* pBuf, int nLen);
	char*	FindDataBegin(char** ppBuf, int& nLen);
	char*	ProcessSocketpacket(char* pBuf, int nLen);
	void	ProcessOneSocketpacket(WORD wIdent, char* pBuf, int nLen);
	BOOL	Dispatch(WORD wID, char* pBuf, int nLen);

private:
	SocketFuncMap	m_SocketFuncMap;
	CSendPacketData	m_SendData;
};
_declspec(selectany) CSocketGame g_socket;


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


#endif // __SocketGame_h__