#ifndef __ZQ_ClientSocketPool_h__
#define   __ZQ_ClientSocketPool_h__
#include "ZQ_CustomIOCPClientSocket.h"

#define  DequeueEvent std::function<bool(CZQ_CustomIOCPClientSocket* ,uint64_t)>
class CZQ_ClientSocketPool
{
public:
	CZQ_ClientSocketPool();
	~CZQ_ClientSocketPool();

	void append(CZQ_CustomIOCPClientSocket* customClientSocket);

	CZQ_CustomIOCPClientSocket * deQueue();
	void clear(DequeueEvent dequeueEvent, uint64_t param);
	int getCount();
private:
	pCustomClientListSocketNode  _firstQueueNode;
	pCustomClientListSocketNode  _lastQueueNode;
	int _count;


};
#endif // __ZQ_ClientSocketPool_h__
