// ------------------------------------------------------------------------- 
//	文件名		: 	TcpClientSocket.h
//	创建者		:	胡小帆
//	创建时间	:	
//	功能描述	:	客户端通讯组件
//
//  详细描述    :
// -------------------------------------------------------------------------
#include "StdAfx.h"
#include "TcpClientSocket.h"

inline void WSAStatupSocket()
{
	WSAData wData;
	WSAStartup(0x0202, &wData);
}

inline void WSACleanupSocket()
{
	WSACleanup();
}

CTcpClientSocket::CTcpClientSocket(CClientSocketCallBackInterface *pCallBackIntf /*= NULL*/) : 
m_CSocket(INVALID_SOCKET),
m_Event(0),
m_bConnected(FALSE),
m_bActive(FALSE),
m_pCallBackIntf(pCallBackIntf),
m_Last(NULL),
m_First(NULL),
m_Count(0),
m_nBackFlags(0)
{
	WSAStatupSocket();
	InitializeCriticalSection(&m_SendCS);
}

CTcpClientSocket::~CTcpClientSocket()
{
	Close();
	DeleteCriticalSection(&m_SendCS);
	WSACleanupSocket();
}

DWORD CTcpClientSocket::SendBuf(const char *pBuf, DWORD Len, BOOL nAsync /* = FALSE */)
{
	DWORD dwRet				= 0;
	BOOL bSendNow			= FALSE;
	SendBufferNode *pNode	= NULL;

	if (m_bConnected && Len > 0) 
	{
		pNode = new SendBufferNode;
		pNode->pNext = NULL;
		pNode->nStart = 0;
		pNode->nBufLen = Len;
		pNode->szBuf = new char[Len];
		memcpy(pNode->szBuf, pBuf, Len);		
		dwRet = Len;

		EnterCriticalSection(&m_SendCS);
		if (m_Last)
			m_Last->pNext = pNode;
		else
		{
			m_First = pNode;
			bSendNow = TRUE;
		}
		
		m_Last = pNode;
		++m_Count;

		LeaveCriticalSection(&m_SendCS);

		if (bSendNow && !nAsync)
			DoSend();
	}

	return dwRet;
}

BOOL CTcpClientSocket::Open(const string &Addr, WORD wPort)
{
	BOOL bRet = FALSE;
	m_Address = Addr;
	m_Port = wPort;
	if (!m_bActive && m_Address.length() > 0 && m_Port != 0)
	{
		SOCKADDR_IN Addr;
		HOSTENT *pHostEnt;

		if (DoInitialize())
		{
			Addr.sin_family = AF_INET;
			Addr.sin_addr.s_addr = inet_addr(m_Address.c_str());
			if (Addr.sin_addr.s_addr == u_long(INADDR_NONE))
			{
				pHostEnt = ::gethostbyname(m_Address.c_str());
				if (pHostEnt)
				{
					//h_addr_list->h_addr_list[0];
					Addr.sin_addr.s_addr = *(u_long *)*(u_long *)(pHostEnt->h_addr_list[0]);
				}
				else
					DoError(seConnect);
			}
		
			Addr.sin_port = htons(m_Port);
			bRet = WSAConnect(m_CSocket, (const sockaddr *)&Addr, sizeof(Addr), NULL, NULL, NULL, NULL) == 0;
			if (bRet) 
			{
				//设置接收缓冲区大小
				INT nRecvBuf = 1024 * 1024;
				setsockopt(m_CSocket, SOL_SOCKET, SO_RCVBUF, (const char *)&nRecvBuf, sizeof(INT));

				m_Event = WSACreateEvent();
				WSAEventSelect(m_CSocket, m_Event, FD_READ | FD_WRITE | FD_CLOSE);
				m_bActive = TRUE;
			}
			else
			{
				closesocket(m_CSocket);
				m_CSocket = INVALID_SOCKET;
			}
		}
	}
	return bRet;
}

BOOL CTcpClientSocket::Close()
{
	BOOL bRet = FALSE;
	if (m_CSocket != INVALID_SOCKET)
	{
		if (shutdown(m_CSocket, SD_BOTH) != SOCKET_ERROR)
			bRet = TRUE;
		else
			DoError(seClose);
		m_bActive = FALSE;


		if (m_Event > 0)
		{
			WSACloseEvent(m_Event);
			m_Event = 0;
		}
		
		closesocket(m_CSocket);
		m_CSocket = INVALID_SOCKET;
		if (m_bConnected)
		{
			m_bConnected = FALSE;
			OnDisconnet();
		}
	}

	EnterCriticalSection(&m_SendCS);
	m_Count = 0;
	SendBufferNode *pNode = NULL;
	while (m_First)
	{
		pNode = m_First;
		m_First = pNode->pNext;
		delete [] pNode->szBuf;
		delete pNode;
	}
	m_Last = NULL;
	LeaveCriticalSection(&m_SendCS);

	return bRet;
}

BOOL CTcpClientSocket::Execute()
{
	BOOL bRet = FALSE;
	INT nCount = 0;
	while (WaitForSingleObject(m_Event, 0) == WAIT_OBJECT_0)
	{
		WSANETWORKEVENTS fdset;
		if (WSAEnumNetworkEvents(m_CSocket, m_Event, &fdset) == 0)
		{
			bRet = TRUE;
			if ((fdset.lNetworkEvents & FD_READ) != 0)
			{
				if (fdset.iErrorCode[FD_READ_BIT] == 0)
					DoRead();
			}

			if ((fdset.lNetworkEvents & FD_WRITE) != 0)
			{
				if (fdset.iErrorCode[FD_WRITE_BIT] == 0)
				{
					DoWrite();
					return bRet;
				}
			}

			if ((fdset.lNetworkEvents & FD_CLOSE) != 0)
			{
				//if (fdset.iErrorCode[FD_CLOSE_BIT] == 0)
					Close();
			}
		}

		++nCount;
		if (nCount == 100) 
			break;
	}

	return bRet;
}

void CTcpClientSocket::DoRead()
{
	const int RECEIVE_BUFFER_SIZE = 16 * 1024;
	WSABUF wsaBuffer;
	DWORD dwTransfer = 0;
	DWORD dwFlags = 0;
	char Buf[RECEIVE_BUFFER_SIZE] = {0};

	wsaBuffer.len = RECEIVE_BUFFER_SIZE;
	wsaBuffer.buf = Buf;
	dwFlags = 0;
	if (WSARecv(m_CSocket, &wsaBuffer, 1, &dwTransfer, &dwFlags, NULL, NULL) == SOCKET_ERROR)
	{
		if (DoError(seRead))
		{
			Close();
			return;
		}
	}
	else 
		OnRead(Buf, dwTransfer);
}

void CTcpClientSocket::DoSend()
{
	WSABUF *pwsaBuffers = NULL;
	WSABUF *pwsaBuf = NULL;
	DWORD dwTransfer;
	SendBufferNode *pNode;

	EnterCriticalSection(&m_SendCS);
	if (m_Count > 0)
	{
		pwsaBuffers = new WSABUF[m_Count];
		pNode = m_First;
		pwsaBuf = pwsaBuffers;
		for (INT i = 0; i<m_Count; ++i)
		{
			pwsaBuf->len = pNode->nBufLen;
			pwsaBuf->buf = pNode->szBuf;
			pNode = pNode->pNext;
			++pwsaBuf;
		}

		dwTransfer = 0;
		if (WSASend(m_CSocket, pwsaBuffers, m_Count, &dwTransfer, 0, NULL, NULL) == SOCKET_ERROR)
		{
			if (DoError(seSend))
				Close();
		}		
		else if (dwTransfer > 0) 
		{
			m_Count = 0;
			m_Last = NULL;
			while (m_First) 
			{
				pNode = m_First;
				m_First = pNode->pNext;
				delete [] pNode->szBuf;
				delete pNode;
			}
		}

		delete []pwsaBuffers;
	}
	LeaveCriticalSection(&m_SendCS);
}

void CTcpClientSocket::OnConnected()
{
	if (m_pCallBackIntf)
		m_pCallBackIntf->OnConnected(m_nBackFlags);
}

void CTcpClientSocket::DoWrite()
{
	if (!m_bConnected) 
	{
		m_bConnected = TRUE;
		OnConnected();
	}
	else
		DoSend();
}

BOOL CTcpClientSocket::DoError( SOCKET_ERROR_TYPE seType )
{
	INT ErrorCode = WSAGetLastError();
	if ((seType & (seRead | seSend)) != 0 && ErrorCode == WSAEWOULDBLOCK) 
		return FALSE;
	
	OnError(ErrorCode);
	return TRUE;
}

BOOL CTcpClientSocket::DoInitialize()
{
	BOOL bRet = FALSE;
	if (m_CSocket == INVALID_SOCKET)
	{
		m_CSocket = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
		bRet = m_CSocket != INVALID_SOCKET;
		if (!bRet) 
			DoError(seConnect);
	}
	return bRet;
}

void CTcpClientSocket::OnRead( char *pBuf, INT nCount )
{
	//OutputDebugStringW(Utils::FormatWStr(L"OnRead %d \n", nCount).c_str());
	if (m_pCallBackIntf)
		m_pCallBackIntf->OnRead(pBuf, nCount);
}

void CTcpClientSocket::OnError( INT &ErrorCode )
{
	if (m_pCallBackIntf)
		m_pCallBackIntf->OnError(ErrorCode);
}

void CTcpClientSocket::OnDisconnet()
{
	if (m_pCallBackIntf)
		m_pCallBackIntf->OnDisconnect(m_nBackFlags);
}

void CTcpClientSocket::SetCallback( CClientSocketCallBackInterface *pCallBackIntf )
{
	m_pCallBackIntf = pCallBackIntf;
}