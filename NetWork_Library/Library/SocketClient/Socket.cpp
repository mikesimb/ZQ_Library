// ------------------------------------------------------------------------- 
//	文件名	: 	Socket.cpp
//	创建者	:	方煜宽
//	创建时间:	2011-12-7 15:59
//	功能描述:	Socket客户端基类
//
// -------------------------------------------------------------------------
#include "stdafx.h"
#include "Socket.h"
#include "../Utils/ErrorCheck.h"

#pragma comment(lib, "ws2_32.lib")

WSAData CSocket::ms_wd;

CSocket::CSocket():
m_hWnd(NULL),
m_socket(INVALID_SOCKET),
m_pSocketCallBack(NULL),
m_bConnected(FALSE)
{
	m_sendBuf.ClearAll();
	m_recvBuf.ClearAll();
}

CSocket::~CSocket()
{
	m_sendBuf.ClearAll();
	m_recvBuf.ClearAll();
}

int CSocket::StartSocket()
{
	return ::WSAStartup(MAKEWORD(2, 2), &CSocket::ms_wd);
}

BOOL CSocket::ConnetToServer(HWND hWnd, LPCSTR ip, INT nPort, long lEvent)
{
	if (INVALID_SOCKET != m_socket)
		return FALSE;

	m_hWnd = hWnd;

	//if (( m_socket = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	if (( m_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 1)) == INVALID_SOCKET)
	{
		L_ASSERT(!"WSASocket");
		return FALSE;
	}

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(ip);
	sockAddr.sin_port = htons(nPort);
	if (::connect(m_socket, (const struct sockaddr FAR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{
		DWORD dwError = WSAGetLastError();
		switch (dwError)
		{
		case WSAEWOULDBLOCK:
			//L_LOG_STR("Non-Blocking Mode");
			OutputDebugStringA("Non-Blocking Mode\r\n");
			break;
		default :
			return FALSE;
		}
	}

	if (::WSAAsyncSelect(m_socket, hWnd, ID_SOCKET_EVENT_MSG, lEvent) == SOCKET_ERROR)
	{
		L_ASSERT(!"WSAAsyncSelect");
		return FALSE;
	}

	return TRUE;
}

BOOL CSocket::DisconnectToServer()
{
	if (INVALID_SOCKET != m_socket)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		m_recvBuf.ClearAll();
		m_sendBuf.ClearAll();
		return TRUE;
	}
	return FALSE;
}

BOOL CSocket::IsConnet()
{
	return INVALID_SOCKET != m_socket;
}

//int SendTest(char* buf, int len)
//{
//	return len / 2;
//	//return len;
//}

void CSocket::Send(char* pData /*= NULL*/, int nDataLen /*= 0*/)
{
	if (0 != m_sendBuf.Length())
	{
		m_sendBuf.Append(pData, nDataLen);
		int nRet = ::send(m_socket, m_sendBuf.Buffer(), m_sendBuf.Length(), 0);
		if (nRet < 0)
		{
			L_LOG_STR("发送网络包失败");
			return;
		}

		m_sendBuf.Remove(nRet);
	}
	else
	{
		if (NULL != pData && 0 != nDataLen)
		{
			int nRet = ::send(m_socket, pData, nDataLen, 0);
			if (nRet < 0)
			{
				L_LOG_STR("发送网络包失败");
				L_LOG_VALUE(WSAGetLastError());
				return;
			}

			if (0 != nDataLen - nRet)
			{
				m_sendBuf.Append(pData + nRet, nDataLen - nRet);
			}
		}
	}
}

BOOL CSocket::OnSocketMessage(WPARAM wParam, LPARAM lParam)
{
	if (!CheckSocketError(lParam))
		return FALSE;

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
		OnConnect((SOCKET) wParam);
		break;
	case FD_CLOSE:
		OnClose((SOCKET) wParam);
		break;
	case FD_READ:
		OnRead((SOCKET) wParam);
		break;
	case FD_WRITE:
		{
			int nTemp = 0;
		}
		break;
	default:
		break;
	}
	return TRUE;
}

void CSocket::SetSocketCallback(SocketCallBack *pSocketCallBack)
{
	m_pSocketCallBack = pSocketCallBack;
}

BOOL CSocket::CheckSocketError(LPARAM lParam)
{
	return TRUE;
}

void CSocket::OnConnect(SOCKET host)
{
	m_bConnected = TRUE;
}

void CSocket::OnClose(SOCKET host)
{
	m_bConnected = FALSE;
	DisconnectToServer();
	if (m_pSocketCallBack)
		m_pSocketCallBack->OnSocketClose();
}

void CSocket::OnRead(SOCKET host)
{
	char szBuf[PACKET_BUF_LEN] = {0};
	int nBufLen = 0;

	nBufLen = recv(host, szBuf, sizeof(szBuf), 0);
	if (nBufLen > 0)
	{
		//m_RecvBuffer.Write((BYTE *)&szBuf[0], nBufLen);
		m_recvBuf.Append(szBuf, nBufLen);
	}
}

void CSocket::Update()
{
	Send();	// 发送上次剩下的网络包
}

BOOL CSocket::GetConnected() const
{
	return m_bConnected;
}