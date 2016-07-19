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
|�������ʼ��ʶ(4�ֽ�)|�����ܳ���(2�ֽ�)|Э���(2�ֽ�)|�����İ����(4�ֽ�)|   |	ûѹ��������	|


���Ident>=0x8000 ��ʾ�˷��Ϊѹ�����ݣ���Ҫȥ����λ�õ���ȷ��Ident				

(Head)													  (Data)
/-------------------------------------------------------------------\   /----------------------------\
|�������ʼ��ʶ(4�ֽ�)|�����ܳ���(2�ֽ�)|Э���(2�ֽ�)|�����İ����(4�ֽ�)|   | ԭʼ����(2�ֽ�) | ѹ�������� |

*/

#pragma pack(1)
typedef struct tagHead
{
	DWORD	dwStartFlag;	// �������ʼ��ʶ 0xAABBCCDD
	WORD	wPackageLen;	// �����ܳ���(������ͷ)
	WORD	wIdent;			// Э��ID(Ident>=0x8000��ʾ�����ݲ��ֱ�ѹ��)
	DWORD	dwAutoID;		// ������ID
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
		return (char*)((BYTE*)m_szBuf + sizeof(Head) + sizeof(WORD)); // sizeof(WORD)Ϊ����ԭʼ���ݳ��ȴ�С�Ŀռ�
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

//�ӽ��ܺ���
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
����ע�� ��Ա������Socket�ص�
ID			: Э��ID
FuncAddr	����Ա������ַ
ע��			����Ա����ֻ����__thiscall����
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