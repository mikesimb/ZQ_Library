// ------------------------------------------------------------------------- 
//	�ļ���		: 	kClientSocket.h
//	������		:	���Ͽ�
//	����ʱ��	:	2011-12-7 15:58
//	��������	:	Socket�ͻ��˻���
//
// -------------------------------------------------------------------------
#ifndef __kClientSocket_h__
#define __kClientSocket_h__

#include <winsock2.h>
#include "../Utils/StreamBuffer.h"
#include "../Client/Buffer.h"

#define ID_SOCKET_EVENT_MSG		(WM_USER + 1000)
#define PACKET_BUF_LEN			65536 // 64k

//////////////////////////////////////////////////////////////////////////
//Socket�ص��ӿ�, ���ڴ��������¼��ص�
struct SocketCallBack
{
	virtual void OnSocketClose() = 0;
};

class CSocket
{
public:
	CSocket();
	virtual ~CSocket();

public:
	static int StartSocket();

	BOOL	ConnetToServer(HWND hWnd, LPCSTR ip, INT nPort, long lEvent = FD_READ | FD_CLOSE);
	BOOL	DisconnectToServer();

	BOOL	IsConnet();

	void	Send(char* pData = NULL, int nDataLen = 0);
	BOOL	OnSocketMessage(WPARAM wParam, LPARAM lParam);		// �յ�win��ϢID_SOCKET_EVENT_MSG�����ñ�����

	void    SetSocketCallback(SocketCallBack *pSocketCallBack);
	BOOL    GetConnected() const;

protected:
	void	Update();

protected:
	virtual BOOL	CheckSocketError(LPARAM lParam);
	virtual void	OnConnect(SOCKET host);
	virtual void	OnClose(SOCKET host);
	virtual void	OnRead(SOCKET host);

protected:
	HWND					m_hWnd;
	CStreamBuffer< char >	m_sendBuf;
	CStreamBuffer< char >	m_recvBuf;

	CBaseBuffer m_RecvBuffer;

private:
	SOCKET			m_socket;
	static WSAData	ms_wd;

	SocketCallBack *m_pSocketCallBack;
	BOOL m_bConnected;

};

#endif // __kClientSocket_h__