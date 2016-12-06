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
		OnErrorEvent(WSAGetLastError());
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

		if (!_isSending)
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

void CZQ_CustomIOCPClientSocket::setActiveTicket(uint64_t value)
{
	_uactiveTick = value;
}

uint64_t CZQ_CustomIOCPClientSocket::getActiveTick()
{
	return _uactiveTick;
}

uint32_t CZQ_CustomIOCPClientSocket::getSocketHandle()
{
	return _socketHandle;
}

void CZQ_CustomIOCPClientSocket::update()
{

}

void CZQ_CustomIOCPClientSocket::OnErrorEvent(int ErrorCode)
{
	if (ErrorCode != ERROR_IO_PENDING && _onSocketErrorEvent != nullptr)
	{
		_onSocketErrorEvent(this, ErrorCode);

	}

}

void CZQ_CustomIOCPClientSocket::initialize()
{
	_isSending = false;
	_data = nullptr;
	_currentTick = 0;
	_totalBufferlen = 0;
	_totalSendDataCount = 0;

}

void CZQ_CustomIOCPClientSocket::finalize()
{
	_isSending = false;
	_data = nullptr;
}

void CZQ_CustomIOCPClientSocket::socketRead(void *buf, int count)
{
	if (_onReviceEvent != nullptr)
	{
		_onReviceEvent(this, buf, count);
	}
}

void CZQ_CustomIOCPClientSocket::doClientRead(pBlock pRBlock, int transfered)
{
if (transfered > 0)
{
	do {
		pRBlock->Buffer[transfered] = 0;
		socketRead(pRBlock->Buffer, transfered);
	} while (0);
	if (_socket != INVALID_SOCKET)
	{
		prepareRecv(pRBlock);
	}
}
}

void CZQ_CustomIOCPClientSocket::doClientSend(pBlock pSBlock, int transfered)
{
	int sendLen = pSBlock->wsaBuffer.len - transfered;
	if (sendLen > 0)
	{
		memcpy(pSBlock->Buffer, &(pSBlock->Buffer[transfered]), sendLen);

	}
	prepareSend(pSBlock, sendLen);

}

void CZQ_CustomIOCPClientSocket::prepareRecv(pBlock pRBlock)
{
	try
	{
		pRBlock->event = ZQ_seRead;
		pRBlock->wsaBuffer.len = MAX_IOCP_BUF_SIZE;
		pRBlock->wsaBuffer.buf = pRBlock->Buffer;
		memset(&(pRBlock->overlapped), 0, sizeof(OVERLAPPED));
		DWORD transfer = 0, flag = 0;
		if (_socket == INVALID_SOCKET || WSARecv(_socket, &(pRBlock->wsaBuffer), 1, &transfer, &flag, &(pRBlock->overlapped),nullptr))
		{
			int errorcode = WSAGetLastError();
			if (errorcode != ERROR_IO_PENDING)
			{
				OnErrorEvent(errorcode);
				close();
			}
		}
	}
	catch (...)
	{
		OnErrorEvent(GetLastError());
	}
}

void CZQ_CustomIOCPClientSocket::prepareSend(pBlock pSBlock, int iSendlen)
{
	EnterCriticalSection(&_sendBufferCS);
	do {
		_isSending = false;
		pSendQueueNode pNode = nullptr;
		int iRemainlen = 0;//这个是什么意思?
		int iDatalen = 0;//这个是什么意思？
		int iErrCode = 0;
		while (_firstQueueNode != nullptr)
		{
			pNode = _firstQueueNode;
			iRemainlen = MAX_IOCP_BUF_SIZE - iSendlen;
			iDatalen = pNode->iBufferLen - pNode->iStartPosition;
			if (iDatalen > iRemainlen)
			{
				memcpy(&(pSBlock->Buffer[iSendlen]), &(pNode->szBuf[pNode->iStartPosition]), iRemainlen);
				iSendlen = MAX_IOCP_BUF_SIZE;
				pNode->iStartPosition += iRemainlen;
				break;
			}
			else
			{
				memcpy(&(pSBlock->Buffer[iSendlen]), &(pNode->szBuf[pNode->iStartPosition]), iRemainlen);
				iSendlen += iDatalen;
				_firstQueueNode = _firstQueueNode->next;
				if (_firstQueueNode == nullptr)
				{
					_lastQueueNode = nullptr;

				}

				free(pNode->szBuf);
				delete(pNode);
				pNode = nullptr;
			}//if
		}//while
		_totalBufferlen -= iSendlen;
		if (iSendlen > 0)
		{
			_isSending = false;
			pSBlock->event = ZQ_seWrite;
			pSBlock->wsaBuffer.len = iSendlen;
			pSBlock->wsaBuffer.buf = pSBlock->Buffer;
			memset(&(pSBlock->Buffer), 0, sizeof(OVERLAPPED));

			DWORD transfer;
			if (_socket == INVALID_SOCKET ||
				WSASend(_socket, &(pSBlock->wsaBuffer), 1, &transfer, 0, &(pSBlock->overlapped), nullptr) == SOCKET_ERROR)
			{
				iErrCode = WSAGetLastError();
				if (iErrCode != ERROR_IO_PENDING)
				{
					OnErrorEvent(iErrCode);
					close();
				}//if
			}//if
		}
	} while (0);
	LeaveCriticalSection(&_sendBufferCS);
}

void CZQ_CustomIOCPClientSocket::clearSendBuffer()
{
	EnterCriticalSection(&_sendBufferCS);
	do {
		pSendQueueNode pNode = nullptr;
		while (_firstQueueNode != nullptr)
		{
			pNode = _firstQueueNode;
			_firstQueueNode = _firstQueueNode->next;
			free(pNode->szBuf);
			delete(pNode);
			pNode = nullptr;
		}
	} while (0);
	LeaveCriticalSection(&_sendBufferCS);
}


