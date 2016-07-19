#include "stdafx.h"
#include "ZQThread.h"



CZQThread::CZQThread()
{
	m_dwThreadID = 0;
	m_hThread = INVALID_HANDLE_VALUE;
	m_bTerminated = TRUE;
	m_bIsSuspended = false;
	ThreadName = "";
}


CZQThread::~CZQThread()
{
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		if (m_bIsSuspended)
		{
			Resume();
		}
		Terminate();
		DWORD time = Max_Wait_for_Timeout;
		WaitFor(time);
	}
}

BOOL CZQThread::Initialize(BOOL bSuspended /*= FALSE*/, unsigned int uStackSize /*= 0*/)
{
	unsigned int dwThreadID;
	int iFlag = 0;
	//���������ô���ù���ı�־
	if (bSuspended)
	{
		iFlag = CREATE_SUSPENDED;
		m_bIsSuspended = true;

	}
	else
	{
		m_bIsSuspended = false;
	}
	//�����߳�
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, uStackSize, ThreadProc, (void*)this, iFlag, &dwThreadID);

	if (hThread == INVALID_HANDLE_VALUE)
		//�������ʧ��
		return false;
	m_dwThreadID = dwThreadID;
	m_hThread = hThread;
	return true;

}

BOOL CZQThread::Suspend(void)
{
	//ִ�й����̲߳�����
	if (SuspendThread(m_hThread) != (DWORD)-1)
	{
		//�ɹ�����
		m_bIsSuspended = TRUE;
		return TRUE;
	}
	else
	{
		//����ʧ��
		m_bIsSuspended = FALSE;
		return FALSE;
	}
}

BOOL CZQThread::Resume(void)
{
	//�����ж�����̴߳��ڷǹ���״̬��ô�˳���
	if (!m_bIsSuspended)
	{
		return TRUE;
	}
	//�ָ��߳�
	if (ResumeThread(m_hThread) != (DWORD)-1)
	{
		//�ָ��ɹ�
		m_bIsSuspended = FALSE;
		return TRUE;
	}
	else
	{
		//�ָ�ʧ��
		m_bIsSuspended = TRUE;
		return  FALSE;
	}
}

void CZQThread::Terminate(void)
{
	//ǿ������m_bTerminated = true;
	::InterlockedExchange((LONG *)(&m_bTerminated), TRUE);


}

void CZQThread::WaitFor(const DWORD dwTimeOut /*= MAX_WAIT_FOR_TIME*/)
{
	//����߳̾����Ч
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		//�ȴ��߳�
		WaitForSingleObject(m_hThread, dwTimeOut);
	}
}

BOOL CZQThread::Terminated(void)
{
	return  m_bTerminated;
}


void CZQThread::SetThreadName(char * name)
{
	ThreadName = name;
}

unsigned WINAPI  CZQThread::ThreadProc(LPVOID pParam)
{
	CZQThread * pThead = (CZQThread *)pParam;
	::InterlockedExchange((LONG *)&(pThead->m_bTerminated), FALSE);
	try
	{


		pThead->Execute();

	}
	catch (...)
	{

	}
	try
	{
		pThead->OnTerminate();
	}
	catch (...)
	{

	}
	_endthreadex(0);
	pThead->m_hThread = INVALID_HANDLE_VALUE;
	pThead->m_dwThreadID = 0;
	pThead->m_bTerminated = true;
	return 0;
}