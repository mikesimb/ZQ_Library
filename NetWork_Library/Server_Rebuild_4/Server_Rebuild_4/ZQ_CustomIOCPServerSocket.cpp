#include "stdafx.h"
#include "ZQ_CustomIOCPServerSocket.h"


CZQ_CustomIOCPServerSocket::CZQ_CustomIOCPServerSocket()
	:_clientCount(0)
	,_hashClientCount(0)
	,_freeClientPool(nullptr)
	,_timeOutCheckTick(0)
	,_acceptThread(nullptr)
	,_currentHandle(1000)
	,OnSocketRevice(nullptr)
	,OnCheckIPAddress(nullptr)
	,OnCreateSocketEvent(nullptr)
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	int processorsCount = sysinfo.dwNumberOfProcessors;
	if (processorsCount >= MAX_WORK_THREAD_SIZE)
		processorsCount = MAX_WORK_THREAD_SIZE - 1;
	

}


CZQ_CustomIOCPServerSocket::~CZQ_CustomIOCPServerSocket()
{
}
