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

	}
}

