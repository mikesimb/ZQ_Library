#include "stdafx.h"
#include "ZQ_CustomIOCPSocket.h"


CZQ_CustomIOCPSocket::CZQ_CustomIOCPSocket()
	:_hIOCP(0)
	,OnSocketError(nullptr)
	,OnSocketSuccess(nullptr)
	,OnSocketDisconnect(nullptr)
{
	_IOCPSOCKET.socket = INVALID_SOCKET;
}


CZQ_CustomIOCPSocket::~CZQ_CustomIOCPSocket()
{
}

HANDLE CZQ_CustomIOCPSocket::getIOCPHandle()
{
	return _hIOCP;
}

IOCPSOCKET * CZQ_CustomIOCPSocket::getIOCPSOCKET()
{
	return &_IOCPSOCKET;
}

void CZQ_CustomIOCPSocket::setServerIP(CZQString IPAddress)
{
	if (_IOCPSOCKET.socket == INVALID_SOCKET && IPAddress.GetLength() <= 0)
	{
		memset(_IOCPSOCKET.IP, 0, 16);
		memcpy(_IOCPSOCKET.IP, (char*)IPAddress, IPAddress.GetLength());

	}
}

int CZQ_CustomIOCPSocket::getServerPort()
{
	return _IOCPSOCKET.port;
}

void CZQ_CustomIOCPSocket::setServerPort(const int Port)
{
	if(_IOCPSOCKET.socket == INVALID_SOCKET)
		 _IOCPSOCKET.port = Port;

}

bool CZQ_CustomIOCPSocket::isAcriveed()
{
	return _IOCPSOCKET.socket != INVALID_SOCKET;

}

void CZQ_CustomIOCPSocket::open()
{
	setActive(true);
}

void CZQ_CustomIOCPSocket::close()
{
	setActive(false);
}

void CZQ_CustomIOCPSocket::socketErrorEvent(CZQ_CustomIOCPClientSocket* clientsocket, int errorCode)
{
	if (OnSocketError != nullptr)
	{
		OnSocketError(clientsocket, errorCode);
	}
	if (errorCode != 0)
	{
		//OutputDebugString("SocketError Code = %d",ErrorCode);
		//这里可以打印错误信息
	}
}

