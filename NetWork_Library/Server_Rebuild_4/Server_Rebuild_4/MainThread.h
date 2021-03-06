#pragma once
#include "ZQThread.h"
#include "MessageServer.h"
class CMainThread :
	public CZQThread
{
public:
	CMainThread();
	virtual ~CMainThread();
	static CMainThread* getInstance();
	static void FreeInstance();
	void SetMainWindowHwnd(HWND value){ value = m_MainWnd; };
	HWND getMainWindowHwnd(){return m_MainWnd; };
	virtual BOOL Initialize(BOOL bSuspended  = FALSE, unsigned int uStackSize  = 0 );
	virtual void Terminate(void);
	bool isRunning() ;
	void setPort(int port) { m_server->setPort(port); };
	int getPort() {return m_server->getPort(); };
	int getServerClientCount() { return m_server->getClientCount(); };
protected:
	virtual void Execute();
	
private:
	void startServer();
	void stopServer();
	HWND   m_MainWnd;
	CMessageServer*  m_server;
};

