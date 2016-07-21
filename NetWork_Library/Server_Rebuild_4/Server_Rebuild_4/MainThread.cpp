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
	//���߳�������ʱ��ʼ����������
	startServer();
	//���߳�ѭ���忪ʼ
	while (!Terminated())
	{
	   //��������Ҫ��ѭ��
		char  MainCount[255];
		sprintf_s(MainCount, 200, " ClientCount: %d\n",m_server->getClientCount() );
		//sprintf_s(MainCount, sizeof(18), "ClientCount is %d \n", m_server.getClientCount());
		//����������������б��л�ȡ����

		//OutputDebugString(MainCount);
		Sleep(10);
	}
	//���߳�ѭ�������
	stopServer();

	 //���߳̽�����ʱ��رշ�����
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