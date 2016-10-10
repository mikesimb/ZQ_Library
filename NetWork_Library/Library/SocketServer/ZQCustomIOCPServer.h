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

	//�������ĸ������еĹ����߳���
	int   m_iWorkThreadCount;	
	pBlock   m_pBlock;
	//�������Ƿ���
	bool m_isRunning;
	//�ͻ����б���ٽ���
	CRITICAL_SECTION  m_ClientCS;
	//����������߳�
	CZQAcceptThread * m_AcceptThread;
	//�����߳�����
	CZQWorkThread ** m_WorkThread;
	//���ڼ���״̬�Ŀͻ����б�
	std::list<CZQCustomServerClient *> m_ClientList;
	//���ڷǼ���״̬�Ŀͻ����б�
	std::list<CZQCustomServerClient *> m_FreeClientList;

	int m_port;
	SOCKET m_socket;
	HANDLE m_hCompletionPort;

	
};

