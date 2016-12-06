#pragma once

#include "IOCPTypeDef.h"
#define MessageQueueTouchProc std::function<void(MsgRef*)>
#define MessageQueueTouchProc2 std::function<void(ServerMsgNode*)>

typedef struct ServerMsgNode
{
	MsgRef * data;
	MsgRef * param;
	ServerMsgNode * next;

	ServerMsgNode()
	{
		data = nullptr;
		param = nullptr;
		next = nullptr;

	}
}ServerMsgNode;

class CZQ_MessageNodeNoLockQueue
{
public:
	CZQ_MessageNodeNoLockQueue();
	virtual ~CZQ_MessageNodeNoLockQueue();

	inline virtual int getCount() { return _count; }
	virtual void addNode(MsgRef* data);
	virtual void addNode(MsgRef* data, MsgRef* param);
	virtual void addNode(int eventID);
	virtual void addNode(int eventID, MsgRef* param);

	virtual void cleanQueue();         

	virtual void touch(MessageQueueTouchProc touchProc);
	virtual void touch(MessageQueueTouchProc2 touchproc);
protected:
	ServerMsgNode * _firstMsgNode = nullptr;
	ServerMsgNode* _lastMsgNode = nullptr;
	int _count = 0;


};

class CZQ_MessageNodeQueue :public CZQ_MessageNodeNoLockQueue
{
public:
	CZQ_MessageNodeQueue();
	virtual ~CZQ_MessageNodeQueue();

	inline virtual int getCount() { return _count; };
	virtual void addNode(MsgRef* data);
	virtual void addNode(MsgRef* data, MsgRef* param);
	virtual void addNode(int eventID);
	virtual void addNode(int eventID, MsgRef* param);

	virtual void cleanQueue();

	virtual void touch(MessageQueueTouchProc touchProc);
	virtual void toucn(MessageQueueTouchProc2  touchProc);
private:
	RTL_CRITICAL_SECTION _dataCS;


};


