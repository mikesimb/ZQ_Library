#include "stdafx.h"
#include "MainThread.h"

static CMainThread * g_mainThread=NULL;

CMainThread::CMainThread()
{
	m_server = new CMessageServer;

}


CMainThread::~CMainThread()
{
	delete(m_server);
	g_mainThread = NULL;
}

void CMainThread::Execute()
{
	//主线程启动的时候开始启动服务器
	startServer();
	//主线程循环体开始
	while (!Terminated())
	{
	   //这里做主要主循环
		char  MainCount[255];
		sprintf_s(MainCount, 200, " ClientCount: %d\n",m_server->getClientCount() );
		//sprintf_s(MainCount, sizeof(18), "ClientCount is %d \n", m_server.getClientCount());
		//这里从主接受数据列表中获取数据

		//OutputDebugString(MainCount);
		Sleep(10);
	}
	//主线程循环体结束
	stopServer();

	 //主线程结束的时候关闭服务器
}

void CMainThread::startServer()
{
	m_server->Initialize();

}

void CMainThread::stopServer()
{
	m_server->Finalize();

}


CMainThread* CMainThread::getInstance()
{
	if (g_mainThread == NULL)
		g_mainThread = new CMainThread();
	return g_mainThread;

}

BOOL CMainThread::Initialize(BOOL bSuspended /* = FALSE */, unsigned int uStackSize /* = 0 */)
{
	CZQThread::Initialize(bSuspended, uStackSize);
	return TRUE;
}

void CMainThread::Terminate(void)
{

	CZQThread::Terminate();
}

bool CMainThread::isRunning()
{
	if (m_server == NULL) 
		 return false;
	return m_server->getIsRuning();
}