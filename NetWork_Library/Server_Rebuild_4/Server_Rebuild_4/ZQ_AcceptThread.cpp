#include "stdafx.h"
#include "ZQ_AcceptThread.h"




CZQ_AcceptThread::CZQ_AcceptThread(CZQ_CustomIOCPServerSocket * parentSocket)
{
	_parentSocket = parentSocket;
	Initialize();

}

CZQ_AcceptThread::~CZQ_AcceptThread()
{
	do {
		if (!Terminated())
			Terminate();
		if (_parentSocket != nullptr)
		{
			if (_parentSocket->getIOCPSOCKET()->socket != INVALID_SOCKET)
			{
				do {
					closesocket(_parentSocket->getIOCPSOCKET()->socket);

				} while (0);
				_parentSocket->getIOCPSOCKET()->socket = INVALID_SOCKET;

			}
		}
		
	}while (0);
	_parentSocket = nullptr;

}

void CZQ_AcceptThread::Execute()
{
	if (_parentSocket == nullptr || _parentSocket->getIOCPSOCKET() == nullptr) return;
	if (listen(_parentSocket->getIOCPSOCKET()->socket, SOMAXCONN) != 0)
	{
		OutputDebugString("Window Socket Error:Listen");
		return;
	}
	int nport = 0;
	int addrlen = sizeof(SOCKADDR_IN);
	SOCKADDR_IN toAddr;
	SOCKET s = 0;

	char ipaddress[16];

	while (!Terminated())
	{
		try {
			if (_parentSocket->getIOCPSOCKET()->socket == INVALID_SOCKET) break;
			addrlen = sizeof(SOCKADDR_IN);
			memset(&toAddr, 0, addrlen);

			s = WSAAccept(_parentSocket->getIOCPSOCKET()->socket,
				(SOCKADDR*)&toAddr,
				(LPINT)&addrlen,
				(LPCONDITIONPROC)&Condition,
				(DWORD_PTR)(_parentSocket));
			if (Terminated()) break;
			if (s != INVALID_SOCKET)
			{
				
				
				strcpy_s(ipaddress, inet_ntoa(toAddr.sin_addr));
				nport = ntohs(toAddr.sin_port);
				_parentSocket->socketAccept(s, ipaddress, nport);
			}
			else
			{
				_parentSocket->socketErrorEvent(nullptr, GetLastError());

			}
		}
		catch(...){
			OutputDebugString("CZQ_AcceptThread.cpp Line 73 is Execption.");
		}
	}
}

int __stdcall Condition(IN LPWSABUF lpCallerId, IN LPWSABUF lpCallerData, IN LPQOS lpSQOS, IN LPQOS lpGQOS, IN LPWSABUF lpCalleeId, IN LPWSABUF lpCalleeData, OUT GROUP FAR * g, IN DWORD_PTR dwCallbackData)
{
	char ipAddress[16];
	auto p = (PSOCKADDR_IN)lpCallerId->buf;
	strcpy_s(ipAddress, inet_ntoa(p->sin_addr));
	int nPort = ntohs(p->sin_port);
	int nIP = inet_addr(ipAddress);
	if (((CZQ_CustomIOCPServerSocket*)dwCallbackData)->isValidAddress(ipAddress, nIP, nPort))
	{
		return CF_ACCEPT;
	}
	else
		return CF_REJECT;
}

CZQ_WorkThread::CZQ_WorkThread(CZQ_CustomIOCPServerSocket* parentSocket, bool boMaster /*= false*/)
{
	_parentSocket = parentSocket;
	_boMaster = boMaster;
	Initialize();


}

CZQ_WorkThread::~CZQ_WorkThread()
{
	while (!Terminated())
	{
		Terminate();
		WaitFor();
	}
}

void CZQ_WorkThread::Execute()
{
	if (_boMaster && !_parentSocket->activestart())
	{
		Sleep(200);
		_parentSocket->activestop();
		return;

	}
	pBlock pblock = nullptr;
	CZQ_CustomIOCPClientSocket* clientsocket = nullptr;
	bool ret = false;
	DWORD dwBytesXfered = 0;
	while (!Terminated())
	{
		try
		{
			pblock = nullptr;
			clientsocket = nullptr;
			if (!_parentSocket->isAcriveed()) break;
			ret = GetQueuedCompletionStatus(_parentSocket->getIOCPHandle(), &dwBytesXfered,
				(PULONG_PTR)&clientsocket,(LPOVERLAPPED*)(&pblock), INFINITE);
			if ((uint32_t)pblock == SHUTDOWN_FLAG)
			{
				Terminate();
			}
			if (Terminated())
			{
				if (clientsocket != nullptr)
					clientsocket->forceClose();
				break;
			}

			if ((uint32_t)pblock == DISCONNECT_FLAG  && clientsocket != nullptr)
			{
				_parentSocket->freeclient(clientsocket);
				continue;
			}//if

			if (!ret || dwBytesXfered == 0)
			{
				if (clientsocket)
					clientsocket->forceClose();
				continue;
			}//if

			if (clientsocket && pblock)
			{
				clientsocket->setActiveTicket(0);
				switch (pblock->event)
				{
				case ZQ_seWrite:
					clientsocket->doClientSend(pblock, dwBytesXfered);
					break;
				case ZQ_seRead:
					clientsocket->doClientRead(pblock, dwBytesXfered);
					break;
				default:
					_parentSocket->socketErrorEvent(clientsocket, 9999);
				}//switch
			}//if
		}//try
		catch (...)
		{
			_parentSocket->socketErrorEvent(clientsocket, GetLastError());
			continue;
		}//catch
	}//while

	PostQueuedCompletionStatus(_parentSocket->getIOCPHandle(), 0, 0,(LPOVERLAPPED)SHUTDOWN_FLAG);
	if (_boMaster)
		_parentSocket->activestop();

}
