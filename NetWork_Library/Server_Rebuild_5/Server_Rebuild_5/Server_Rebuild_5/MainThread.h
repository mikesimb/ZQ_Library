#pragma once
#include "ZQThread.h"
#include "ZQ_MainServer.h"
class CMainThread:public CZQThread
{
public:
	CMainThread();
	~CMainThread();

	void Execute();

	static CMainThread* getInstance()
	{
		if (m_Instance = nullptr)
		{
			m_Instance = new CMainThread();
		}
		return m_Instance;
	};
private:
	static CMainThread * m_Instance;

};

