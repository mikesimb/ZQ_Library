#include "stdafx.h"
#include "ZQ_CustomIOCPClientSocket.h"


CZQ_CustomIOCPClientSocket::CZQ_CustomIOCPClientSocket()
{
}


CZQ_CustomIOCPClientSocket::~CZQ_CustomIOCPClientSocket()
{
}

void CZQ_CustomIOCPClientSocket::close()
{
	if (forceClose() != 0)
	{
		onErrorEvent(WSAGetLastError());
	}
}

int CZQ_CustomIOCPClientSocket::sendBuf(char* buf, int count, bool freeAfterSend /*= false*/)
{
	int result = 0;
	if (count <= 0 || _socket == INVALID_SOCKET)
	{
		if (freeAfterSend)
			free(buf);

		return result;
	}
	result = count;

	pSendQueueNode pNode = new SendQueueNode;

	pNode->iBufferLen = count;
	pNode->szBuf = (char*)malloc(count);
	memcpy(pNode->szBuf, buf, count);
	if (freeAfterSend)
	{
		free(buf);
	}
	pNode->iStartPosition = 0;
	pNode->next = nullptr;
	EnterCriticalSection(&_sendBufferCS);
	do{
		_totalSendDataCount += count;
		_totalBufferlen += count;
		
		if (_lastQueueNode != nullptr)
		{
			_lastQueueNode->next = pNode;

		}

		if (_firstQueueNode != nullptr)
		{
			_firstQueueNode->next = nullptr;

		}
		_lastQueueNode = pNode;

		if (!isSending)
		{
			prepareSend(&_sendBlock, 0);
		}

	} while (0);
	LeaveCriticalSection(&_sendBufferCS);
	return result;
}

int CZQ_CustomIOCPClientSocket::sendText(CZQString s)
{
	int strlen = s.GetLength();
	if (strlen > 0)
	{
		char* buf = (char*)malloc(strlen);
		memcpy(buf, s, strlen);
		return sendBuf(buf, strlen, true);
	}
	else
		return 0;
}

CZQString CZQ_CustomIOCPClientSocket::getRemoteAddress()
{
	return _remoteAddress;
}

int CZQ_CustomIOCPClientSocket::getRemotePort()
{
	return _remotePort;
}

char* CZQ_CustomIOCPClientSocket::getData()
{
	return _data;

}

int CZQ_CustomIOCPClientSocket::getSendBufferLen()
{
	return _totalBufferlen;
}

uint32_t CZQ_CustomIOCPClientSocket::getTotalSendCount()
{
	return _totalSendDataCount;
}

void CZQ_CustomIOCPClientSocket::setTotalSendCount(uint32_t value)
{
	_totalSendDataCount = value;
}

int CZQ_CustomIOCPClientSocket::forceClose()
{
	int  result = 0;
	if (_socket != INVALID_SOCKET)
	{
		do {
			result = closesocket(_socket);

		} while (0);
		_socket = INVALID_SOCKET;

		clearSendBuffer();
		memset(&_readBlock, 0, sizeof(Block));
		PostQueuedCompletionStatus(_hIOCP, 0, (DWORD)(this), (LPOVERLAPPED)DISCONNECT_FLAG);

	}
	return result;
}


