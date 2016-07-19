#pragma once
#include <WinSock2.h>
#include <list>
#include <functional>
#include "ZQNetCommon.h"
#include "ZQCustomServerClient.h"

class CZQCustomServerClient;

typedef std::function<void()>  OnSocketNotifyEvent;
typedef std::function<void(CZQCustomServerClient * client)>  OnSocketConnectNotifyEvent;
typedef std::function<void(char * buf, int buflen)>  OnSocketReadNotifyEvent;
typedef std::function<void(char * buf, int buflen)>  OnSocketWriteNotifyEvent;
typedef std::function<CZQCustomServerClient * ()> OnCreateSocketClientNotifyEvent;



class CZQCustomIOCPServer
{

	friend class CZQAcceptThread;
	friend class CZQWorkThread;
	friend class CZQCustomClient;
public:
	CZQCustomIOCPServer();
	virtual ~CZQCustomIOCPServer();
    
	virtual void Initialize();
	virtual void Finalize();
	int getClientCount();

	bool getIsRuning() { return m_isRunning; };

	void setPort(int port) { m_port = port; };
	int  getPort() { return m_port; };

protected:
	void SocketAccept(SOCKET  s, sockaddr_in addr);
	void FreeClient(CZQCustomServerClient * client);
	void FreeAllClient();
	void ReleaseFreeClientList();
	void CheckClientActived();

	

//	virtual  CZQCustomClient * OnCreateClient();
	OnCreateSocketClientNotifyEvent  m_CreateSocketClient;
private:
	 //·þÎñÆ÷µÄ¼àÌýSOCKET;
	SOCKET  m_socket;
	char      m_address[16];
	int         m_port;
	HANDLE   m_hCompletionPort;
	int   m_iWorkThreadCount;
	pBlock   m_pBlock;
	bool m_isRunning;

	CRITICAL_SECTION  m_ClientCS;

	CZQAcceptThread * m_AcceptThread;
	CZQWorkThread ** m_WorkThread;

	std::list<CZQCustomServerClient *> m_ClientList;
	std::list<CZQCustomServerClient *> m_FreeClientList;

	
};

