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

	//服务器的负责运行的工作线程数
	int   m_iWorkThreadCount;	
	pBlock   m_pBlock;
	//服务器是否开启
	bool m_isRunning;
	//客户端列表的临界区
	CRITICAL_SECTION  m_ClientCS;
	//负责监听的线程
	CZQAcceptThread * m_AcceptThread;
	//工作线程数组
	CZQWorkThread ** m_WorkThread;
	//处于激活状态的客户端列表
	std::list<CZQCustomServerClient *> m_ClientList;
	//处于非激活状态的客户端列表
	std::list<CZQCustomServerClient *> m_FreeClientList;

	int m_port;
	SOCKET m_socket;
	HANDLE m_hCompletionPort;

	
};

