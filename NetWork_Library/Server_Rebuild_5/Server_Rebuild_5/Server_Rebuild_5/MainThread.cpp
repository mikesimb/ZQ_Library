#include "stdafx.h"
#include "MainThread.h"

static CMainThread* m_Instance = nullptr;

CMainThread::CMainThread()
{
}


CMainThread::~CMainThread()
{
}

void CMainThread::Execute()
{
	CZQ_MainServer::getInstance()->StartServer(8879);
	while (Terminated())
	{
		Sleep(10);
	}



}
