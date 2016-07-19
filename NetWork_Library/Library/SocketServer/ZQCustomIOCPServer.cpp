
#include "stdafx.h"
#include "ZQCustomIOCPServer.h"
#include "ZQAcceptThread.h"
#include "ZQWorkThread.h"
#include "ZQCustomClient.h"



CZQCustomIOCPServer::CZQCustomIOCPServer()
{
	WSADATA  wsadata;
	WSAStartup(0x0202, &wsadata);
	m_isRunning = false;
	InitializeCriticalSection(&m_ClientCS);
	m_port = 7100;
}


CZQCustomIOCPServer::~CZQCustomIOCPServer()
{
	WSACleanup();
	DeleteCriticalSection(&m_ClientCS);
}

void CZQCustomIOCPServer::Initialize()
{
   //首先创建一个完成端口
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	//因为完成端口需要若干的工作线程
	SYSTEM_INFO  systeminfo;
	GetSystemInfo(&systeminfo);
	m_AcceptThread = new CZQAcceptThread(this);
	m_iWorkThreadCount = systeminfo.dwNumberOfProcessors;
	m_WorkThread = new CZQWorkThread*[m_iWorkThreadCount];
	//这里需要创建工作线程
	for (int i = 0; i < m_iWorkThreadCount; i++)
	{
		//创建工作线程
		m_WorkThread[i] = new CZQWorkThread(this);

	}
	//这个是监听的SOCKET
	
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket != INVALID_SOCKET)
	{
		SOCKADDR_IN addr;
		memset(&addr, 0, sizeof(SOCKADDR_IN));
		addr.sin_family = PF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;

		WSAHtons(m_socket, m_port, &addr.sin_port);
		if (bind(m_socket, (sockaddr*)&addr, sizeof(SOCKADDR_IN)) == 0)
		{
			//接下来的监听工作实际上可以让ACCEPTTHREAD来做。
			if (listen(m_socket, SOMAXCONN))
			{
				OutputDebugString("Listen function is Error!");
			}
			m_isRunning = true;
			m_AcceptThread->Initialize();
			for (int i = 0; i < m_iWorkThreadCount; i++)
			{
				//创建工作线程
				m_WorkThread[i]->Initialize();

			}
		}
	}
}

void CZQCustomIOCPServer::Finalize()
{
	//PostQueuedCompletionStatus(m_hCompletionPort, 0xFFFFFFFF, 0, NULL);
	FreeAllClient();
	ReleaseFreeClientList();
	closesocket(m_socket);
	m_AcceptThread->Terminate();
	m_AcceptThread->WaitFor();
	delete(m_AcceptThread);
	PostQueuedCompletionStatus(m_hCompletionPort, 0, 0, LPOVERLAPPED(SHUTDOWN_FLAG));

	for (int i = 0; i < m_iWorkThreadCount; i++)
	{
		//创建工作线程
		m_WorkThread[i]->Terminate();
		m_WorkThread[i]->WaitFor();
		delete(m_WorkThread[i]);

	}
	delete (m_WorkThread);

	if (m_hCompletionPort != INVALID_HANDLE_VALUE)
		CloseHandle(m_hCompletionPort);
	if (m_socket != INVALID_SOCKET)
		closesocket(m_socket);

	m_isRunning = false;
}

void CZQCustomIOCPServer::SocketAccept(SOCKET  s, sockaddr_in addr)
{
	if (s != INVALID_SOCKET)
	{
		//这里需要留下自定义的方式来写
		CZQCustomServerClient  * Client;
		if (m_CreateSocketClient)
			Client = m_CreateSocketClient();
		else
			Client = new CZQCustomServerClient();
		Client->m_socket = s;
		Client->m_IOCP = m_hCompletionPort;
		//strcpy_s(Client->m_address, 16, inet_ntoa(addr.sin_addr));
		// vs2015里用inte_ntop替换


		//strcpy_s(Client->m_address, 16, inet_ntop(addr.sin_addr));
		
		Client->m_Port = ntohs(addr.sin_port);
		EnterCriticalSection(&m_ClientCS);
		m_ClientList.push_back(Client);
		LeaveCriticalSection(&m_ClientCS);
		if (CreateIoCompletionPort((HANDLE)s, m_hCompletionPort, (ULONG_PTR)Client, 0) == 0)
		{
			OutputDebugString("function SocketAccept(SOCKET  s, sockaddr_in addr) at CreateIOColpletionPort is failed!");
		}
		else
		{
			Client->ReadyReviceNextData(&Client->m_ReviceData);
		}
	}
	
}

void CZQCustomIOCPServer::FreeClient(CZQCustomServerClient * client)
{
	std::list<CZQCustomServerClient*>::iterator it;
	for (it = m_ClientList.begin(); it != m_ClientList.end(); it++)
	{
		if (*it == client)
		{
			client->ForceClose();
			m_ClientList.erase(it);
			break;
		}

	}
}

void CZQCustomIOCPServer::FreeAllClient()
{
	std::list<CZQCustomServerClient*>::iterator it;
	if (m_ClientList.size() == 0) return;
	for (it = m_ClientList.begin(); it != m_ClientList.end();)
	{
		EnterCriticalSection(&m_ClientCS);
		CZQCustomServerClient * client = *it;
		m_ClientList.erase(it);
		it = m_ClientList.begin();
		delete(client);
		LeaveCriticalSection(&m_ClientCS);

	}
	//m_ClientList.clear();
}

void CZQCustomIOCPServer::CheckClientActived()
{
	std::list<CZQCustomServerClient*>::iterator it;

	for (it = m_ClientList.begin(); it != m_ClientList.end();)
	{
		EnterCriticalSection(&m_ClientCS);
		CZQCustomServerClient * client = *it;
		if (GetTickCount() - client->m_dwActiveTick > 500)
			//说明客户端有问题 
			m_ClientList.erase(it);
		m_FreeClientList.push_back(client);
		LeaveCriticalSection(&m_ClientCS);

	}

}

void CZQCustomIOCPServer::ReleaseFreeClientList()
{
	std::list<CZQCustomServerClient*>::iterator it;
	if (m_ClientList.size() == 0) return ;
	for (it = m_FreeClientList.begin(); it != m_FreeClientList.end();)
	{
		EnterCriticalSection(&m_ClientCS);
		CZQCustomClient * client = *it;
			//说明客户端有问题 
		m_FreeClientList.erase(it);
		delete client;
		LeaveCriticalSection(&m_ClientCS);

	}
}

int CZQCustomIOCPServer::getClientCount()
{
	return m_ClientList.size();
}
