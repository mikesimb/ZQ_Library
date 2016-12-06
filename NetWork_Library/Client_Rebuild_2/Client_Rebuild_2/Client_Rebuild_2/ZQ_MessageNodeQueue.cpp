#include "stdafx.h"
#include "ZQ_MessageNodeQueue.h"

CZQ_MessageNodeNoLockQueue::CZQ_MessageNodeNoLockQueue()
{
	
}

CZQ_MessageNodeNoLockQueue::~CZQ_MessageNodeNoLockQueue()
{
	cleanQueue();
}

void CZQ_MessageNodeNoLockQueue::addNode(MsgRef* data)
{
	ServerMsgNode * pNode = new ServerMsgNode;
	MsgRef * tmpMsg = (MsgRef*)malloc(data->datalength);
	memcpy(tmpMsg, data, data->datalength);
	pNode->data = tmpMsg;
	pNode->next = nullptr;

	do {
		_count++;
		if (_lastMsgNode != nullptr)
		{
			_lastMsgNode->next = pNode;
		}

		if (_firstMsgNode == nullptr)
		{
			_firstMsgNode = pNode;
		}
		_lastMsgNode = pNode;
	} while (0);

}

void CZQ_MessageNodeNoLockQueue::addNode(int eventID)
{
	ServerMsgNode * pNode = new ServerMsgNode;
	MsgRef * tmpMsg = new MsgRef();
	tmpMsg->cmd = eventID;
	pNode->data = tmpMsg;
	pNode->next = nullptr;

	do {
		_count++;
		if (_lastMsgNode != nullptr)
		{
			_lastMsgNode->next = pNode;

		}

		if (_firstMsgNode == nullptr)
		{
			_firstMsgNode = pNode;
		}
		_lastMsgNode = pNode;
	} while (0);
}

void CZQ_MessageNodeNoLockQueue::addNode(MsgRef* data, MsgRef* param)
{
	ServerMsgNode * pNode = new ServerMsgNode;
	MsgRef * tmpMsg = (MsgRef*)malloc(data->datalength);
	memcpy(tmpMsg, data, data->datalength);
	pNode->data = tmpMsg;

	if (param)
	{
		pNode->param = (MsgRef*)malloc(param->datalength);
		memcpy(pNode->param, param, param->datalength);

	}
	else
		pNode->param = nullptr;
	pNode->next = nullptr;

	do {
		_count++;
		if (_lastMsgNode != nullptr)
		{
			_lastMsgNode->next = pNode;

		}

		if (_firstMsgNode == nullptr)
		{
			_firstMsgNode = pNode;
		}
		_lastMsgNode = pNode;

	} while (0);

}

void CZQ_MessageNodeNoLockQueue::addNode(int eventID, MsgRef* param)
{
	ServerMsgNode * pNode = new ServerMsgNode;
	MsgRef * tmpMsg = new MsgRef();
	tmpMsg->cmd = eventID;
	pNode->data = tmpMsg;
	if (param)
	{
		pNode->param = (MsgRef *)malloc(param->datalength);
		memcpy(pNode->param, param, param->datalength);

	}
	else
		pNode->param = nullptr;

	pNode->next = nullptr;

	do {
		_count++;
			if (_lastMsgNode != nullptr) {
				_lastMsgNode->next = pNode;
			}
		if (_firstMsgNode != nullptr)
		{
			_firstMsgNode = pNode;
		}
		_lastMsgNode = pNode;
	} while (0);
}

void CZQ_MessageNodeNoLockQueue::cleanQueue()
{
	do {
		ServerMsgNode* pNode = nullptr;
		while (_firstMsgNode != nullptr)
		{
			pNode = _firstMsgNode;
			_firstMsgNode = _firstMsgNode->next;
			free(pNode->data);
			delete pNode;
			pNode = nullptr;

		}
		_lastMsgNode = nullptr;
		_count = 0;

	} while (0);
}

void CZQ_MessageNodeNoLockQueue::touch(MessageQueueTouchProc touchProc)
{
	do {
		ServerMsgNode* pNode = nullptr;
		while (_firstMsgNode != nullptr)
		{
			pNode = _firstMsgNode;
			_firstMsgNode = _firstMsgNode->next;
			if (touchProc)
				touchProc(pNode->data);
			if (pNode->param)
				free(pNode->data);
			free(pNode->data);
			delete pNode;
			pNode = nullptr;
		}
		_lastMsgNode = nullptr;
		_count = 0;
	} while (0);
}

void CZQ_MessageNodeNoLockQueue::touch(MessageQueueTouchProc2 touchproc)
{
  do 
  {
	  ServerMsgNode * pNode = nullptr;
	  while (_firstMsgNode != nullptr)
	  {
		  pNode = _firstMsgNode;
		  _firstMsgNode = _firstMsgNode->next;
		  if (touchproc)
			  touchproc(pNode);
		  if (pNode->param)
			  free(pNode->param);

		  free(pNode->data);
		  delete pNode;
		  pNode = nullptr;
	  }
	  _lastMsgNode = nullptr;
	  _count = 0;
  } while (0);
}

CZQ_MessageNodeQueue::CZQ_MessageNodeQueue()
{
	InitializeCriticalSection(&_dataCS);
}

CZQ_MessageNodeQueue::~CZQ_MessageNodeQueue()
{
	cleanQueue();
	DeleteCriticalSection(&_dataCS);
}

void CZQ_MessageNodeQueue::addNode(MsgRef* data)
{
	EnterCriticalSection(&_dataCS);
	CZQ_MessageNodeNoLockQueue::addNode(data);
	LeaveCriticalSection(&_dataCS);
}

void CZQ_MessageNodeQueue::addNode(int eventID)
{
	EnterCriticalSection(&_dataCS);

	CZQ_MessageNodeNoLockQueue::addNode(eventID);

	LeaveCriticalSection(&_dataCS);
}

void CZQ_MessageNodeQueue::addNode(MsgRef* data, MsgRef* param)
{
	EnterCriticalSection(&_dataCS);

	CZQ_MessageNodeNoLockQueue::addNode(data,param);

	LeaveCriticalSection(&_dataCS);

}

void CZQ_MessageNodeQueue::addNode(int eventID, MsgRef* param)
{
	EnterCriticalSection(&_dataCS);

	CZQ_MessageNodeNoLockQueue::addNode(eventID, param);

	LeaveCriticalSection(&_dataCS);

}

void CZQ_MessageNodeQueue::cleanQueue()
{
	EnterCriticalSection(&_dataCS);

	CZQ_MessageNodeNoLockQueue::cleanQueue();

	LeaveCriticalSection(&_dataCS);

}

void CZQ_MessageNodeQueue::touch(MessageQueueTouchProc touchProc)
{
	EnterCriticalSection(&_dataCS);

	CZQ_MessageNodeNoLockQueue::touch(touchProc);

	LeaveCriticalSection(&_dataCS);

}

void CZQ_MessageNodeQueue::toucn(MessageQueueTouchProc2 touchProc)
{
	EnterCriticalSection(&_dataCS);

	CZQ_MessageNodeNoLockQueue::touch(touchProc);

	LeaveCriticalSection(&_dataCS);

}
