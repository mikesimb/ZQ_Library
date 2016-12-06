#include "stdafx.h"
#include "ZQ_MainServer.h"


static CZQ_MainServer* m_pInstance = nullptr;

CZQ_MainServer::CZQ_MainServer()
{
	WSAStartupSocket();
	OnCreateSocketEvent = IOCP_EVENT_CALLBACK_0(CZQ_MainServer::CreateNewClientSocket, this);
	OnSocketError = IOCP_EVENT_CALLBACK_2(CZQ_MainServer::ClientSocketError, this);
	OnDisConnectCallBack = IOCP_EVENT_CALLBACK_1(CZQ_MainServer::ClientSocketDisconnect, this);
	OnConnectCallBack = IOCP_EVENT_CALLBACK_1(CZQ_MainServer::ClientSocketConnect, this);
}


CZQ_MainServer::~CZQ_MainServer()
{
	if (isActiveed())
	{
		StopServer();
	}
	WSACleanupSocket();
}

CZQ_MainServer* CZQ_MainServer::getInstance()
{
	if (m_pInstance == nullptr)
		m_pInstance = new CZQ_MainServer();
	return m_pInstance;
};

CZQ_CustomIOCPClientSocket* CZQ_MainServer::CreateNewClientSocket()
{
	OutputDebugString("Create New Client.......");

	CZQ_CustomIOCPClientSocket* client = new  CZQ_CustomIOCPClientSocket();

	return client;


}

void CZQ_MainServer::ClientSocketError(CZQ_CustomIOCPClientSocket * clientsocket, int & errorcode)
{
	OutputDebugString("ClientSocketError ....................");
}

void CZQ_MainServer::ClientSocketConnect(CZQ_CustomIOCPClientSocket* clientsocket)
{
	OutputDebugString("Have Client Connected  ....................");
}

void CZQ_MainServer::ClientSocketDisconnect(CZQ_CustomIOCPClientSocket* clientsocket)
{
	OutputDebugString("Have Client DisConnected  ....................");
}

void CZQ_MainServer::StartServer(int port)
{
	if (isActiveed() && !_isRequestListen)
	{
		setServerIP("0,0,0,0");
		setServerPort(7809);
		open();
		_isRequestListen = true;
	}
}

void CZQ_MainServer::StopServer()
{
	if (isActiveed())
	{
		close();
		_isRequestListen = false;

	}
}

void CZQ_MainServer::update()
{
	lock();
	do
	{
		for (size_t i = 0; i < getActiveConnects()->size(); i++)
		{
			auto client = static_cast<CZQ_CustomIOCPClientSocket*> (getActiveConnects()->at(i));
			client->update();
		}
	} while (0);
	unlock();
}


