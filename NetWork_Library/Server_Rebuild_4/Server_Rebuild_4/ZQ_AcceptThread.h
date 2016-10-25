#ifndef __ZQ_AcceptThread_H__
#define __ZQ_AcceptThread_H__
#include "ZQThread.h"
#include "IOCPTypeDef.h"
#include "ZQ_CustomIOCPServerSocket.h"
using namespace std;

class CZQ_AcceptThread :
	public CZQThread
{
public:
	CZQ_AcceptThread(CZQ_CustomIOCPServerSocket * parentSocket);
	virtual ~CZQ_AcceptThread();

	void Execute();
private:
	CZQ_CustomIOCPServerSocket* _parentSocket;



};


class CZQ_WorkThread :public CZQThread
{
public:
	CZQ_WorkThread(CZQ_CustomIOCPServerSocket* parentSocket, bool boMaster = false);
	virtual ~CZQ_WorkThread();

	void Execute();
private:
	bool _boMaster;
	CZQ_CustomIOCPServerSocket* _parentSocket;

};


int __stdcall Condition(
	IN LPWSABUF lpCallerId,
	IN LPWSABUF lpCallerData,
	IN LPQOS lpSQOS,
	IN LPQOS lpGQOS,
	IN LPWSABUF lpCalleeId,
	IN LPWSABUF lpCalleeData,
	OUT GROUP FAR * g,
	IN DWORD_PTR dwCallbackData);

#endif // __ZQ_AcceptThread_H__


