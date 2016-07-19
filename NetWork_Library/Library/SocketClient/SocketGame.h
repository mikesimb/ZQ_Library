// ------------------------------------------------------------------------- 
//	�ļ���	: 	SocketGame.h
//	������	:	���Ͽ�
//	����ʱ��:	2011-12-8 16:13
//	��������:	��Ϸ�ͻ���Socket
//  ע��	: 	������ͷ�ļ�(GameSocket.h)����<windows.h>ǰ����
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

class CSocketGame : public CSocket
{
public:
	typedef map< WORD, MemberFuncInfo > SocketFuncMap;
	
public:
	CSocketGame();
	virtual ~CSocketGame();

	BOOL	SendData(WORD wID, LPCSTR pData = NULL, int nDataLen = 0);
	void	ProcessSocketpacket();											// ��Ϸѭ�������
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
	����ע�� ��Ա������Socket�ص�
	ID			: Э��ID
	FuncAddr	����Ա������ַ
	ע��			����Ա����ֻ����__thiscall����
*/
#define RegSocketFunc(ID, MemberFuncAddr)			\
info.dwThis = (DWORD)this;							\
GetMemberFuncAddr(info.dwFuncAddr, MemberFuncAddr);	\
g_socket.RegFunc(ID, info);


#endif // __SocketGame_h__