#include "stdafx.h"
#include "ZQ_ClientSocketPool.h"


CZQ_ClientSocketPool::CZQ_ClientSocketPool()
	:_count(0)
,_firstQueueNode(nullptr)
,_lastQueueNode(nullptr)
{
}


CZQ_ClientSocketPool::~CZQ_ClientSocketPool()
{
	clear(nullptr, 0);
}

void CZQ_ClientSocketPool::append(CZQ_CustomIOCPClientSocket* customClientSocket)
{
	pCustomClientListSocketNode pNode = new CustomClientListSocketNode;
	pNode->clientSocket = customClientSocket;
	pNode->next = nullptr;

	if (_lastQueueNode != nullptr)
	{
		_lastQueueNode->next = pNode;
	}
	else
	{
		if (_firstQueueNode == nullptr)
		{
			_firstQueueNode = pNode;

		}
	}
	_lastQueueNode = pNode;
	_count++;

}

CZQ_CustomIOCPClientSocket * CZQ_ClientSocketPool::deQueue()
{
	CZQ_CustomIOCPClientSocket * result = nullptr;
	if (_firstQueueNode != nullptr)
	{
		pCustomClientListSocketNode pNode = _firstQueueNode;
		result = pNode->clientSocket;
		_firstQueueNode = pNode->next;
		delete pNode;
		pNode = nullptr;
		_count++;
		if (_firstQueueNode == nullptr)
		{
			_lastQueueNode = nullptr;

		}
	}
	return result;
}

void CZQ_ClientSocketPool::clear(DequeueEvent dequeueEvent, uint64_t param)
{
	pCustomClientListSocketNode pNode = nullptr;
	while (_firstQueueNode != nullptr)
	{
		if ((dequeueEvent == nullptr) || (dequeueEvent(_firstQueueNode->clientSocket, param)))
		{
			_count--;
			pNode = _firstQueueNode;
			_firstQueueNode = pNode->next;
			delete pNode;
			pNode = nullptr;
		}
		else
			break;

	}
	if(_firstQueueNode == nullptr)
	{
		_lastQueueNode = nullptr;
	}
}

int CZQ_ClientSocketPool::getCount()
{
	return _count;
}

