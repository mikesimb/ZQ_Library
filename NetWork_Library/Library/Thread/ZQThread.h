#pragma once

/*
����һ���߳���CZQThread.
���ߣ�����ΰ
���߳���Ŀǰ���߱��߳�ͬ������
*/

#include <windows.h>
#include <string>

#define Max_Wait_for_Timeout    6000



class CZQThread
{
public:
	CZQThread();
	virtual ~CZQThread();

	

public:
	//�����߳�
	virtual BOOL  Initialize(BOOL bSuspended = FALSE, unsigned int uStackSize = 0);

	//�̹߳���
	BOOL Suspend(void);

	//�ָ��߳�
	BOOL Resume(void);

	//��ֹ�߳�
	virtual void Terminate(void);

	//�ȴ��߳�
	void WaitFor(const DWORD dwTimeOut = Max_Wait_for_Timeout);

	const char * getThreadID() { return (char *)m_dwThreadID; }
	const char * getThreadHandle(){ return (char *)m_hThread; }
	HANDLE  getThread() { return m_hThread; };
	//�߳��Ƿ���ֹ
	BOOL Terminated(void);

	//�����߳�����
	void SetThreadName(char * name);



protected:


	//ִ�й���
	virtual void Execute(void) = 0;
	//��ֹ�¼�
	virtual void OnTerminate(void) { OutputDebugString(ThreadName.c_str()); };

	volatile BOOL m_bTerminated;

private:
	//�߳�ID
	DWORD m_dwThreadID;
	//�߳̾��
	HANDLE m_hThread;
	//��ǰ�Ƿ����
	BOOL m_bIsSuspended;
	//�߳�ͬ���¼�

	//�̵߳�����
	std::string ThreadName;

	//�̹߳���
	static unsigned WINAPI ThreadProc(LPVOID pParam);
};
