
#include "stdafx.h"
#include "ZQ_ClientIOCPSocket.h"



CZQ_ClientIOCPSocket::CZQ_ClientIOCPSocket()
	:_activeSocket(nullptr)
	,_workThread(nullptr)
{
	_activeSocket = new CZQ_CustomIOCPClientSocket();
	_activeSocket->_hIOCP = _hIOCP;
	_activeSocket->_onSocketErrorEvent = std::bind(&CZQ_CustomIOCPSocket::socketErrorEvent, this, std::placeholders::_1, std::placeholders::_2);

}

CZQ_ClientIOCPSocket::~CZQ_ClientIOCPSocket()
{
	_activeSocket->forceClose();
	setActive(false);
	delete _activeSocket;
	_activeSocket = nullptr;
}

CZQ_CustomIOCPClientSocket * CZQ_ClientIOCPSocket::getSocket()
{
	return _activeSocket;
}

void CZQ_ClientIOCPSocket::setOnReadEvent(SocketReviceEvent readEvent)
{

	_activeSocket->_onReviceEvent = readEvent;
}

void CZQ_ClientIOCPSocket::setActive(bool value)
{
	if (value)
	{
		if (_IOCPSOCKET.socket != INVALID_SOCKET ||
			_IOCPSOCKET.port == 0 ||
			(inet_addr(_IOCPSOCKET.IP) == INADDR_ANY))
		{
			return;
		}
		if (_workThread)
		{
			_workThread->Terminate();
			PostQueuedCompletionStatus(_hIOCP, 0, 0, LPOVERLAPPED(SHUTDOWN_FLAG));
			delete _workThread;

		}
		_workThread = new CZQ_WorkThread(this,false);
	}
	else
	{
		if (_workThread)
		{
			_workThread->Terminate();
			PostQueuedCompletionStatus(_hIOCP, 0, 0, LPOVERLAPPED(SHUTDOWN_FLAG));
			delete _workThread;
			_workThread = nullptr;
			activestop();

		}
		_IOCPSOCKET.socket = INVALID_SOCKET;
	}

}

void CZQ_ClientIOCPSocket::freeclient(CZQ_CustomIOCPClientSocket* clientSocket)
{
	if (clientSocket == _activeSocket
		&& _IOCPSOCKET.socket != INVALID_SOCKET)
	{
		_IOCPSOCKET.socket = INVALID_SOCKET;
		if (OnDisConnectCallBack)
		{
			OnDisConnectCallBack(clientSocket);
		}
	}
}

bool CZQ_ClientIOCPSocket::activestart()
{
	bool result = false;
	if (_IOCPSOCKET.socket != INVALID_SOCKET
		|| _IOCPSOCKET.port == 0
		|| (inet_addr(_IOCPSOCKET.IP) == INADDR_NONE)
		)
	{
		return result;
	}

	try
	{
		//初始化
		_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		if (_hIOCP != 0)
		{
			_IOCPSOCKET.socket = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_IP,
				nullptr, 0, WSA_FLAG_OVERLAPPED);
			if (_IOCPSOCKET.socket != INVALID_SOCKET)
			{
				SOCKADDR_IN toAddr;
				memset(&toAddr, 0, sizeof(SOCKADDR_IN));
				toAddr.sin_family = PF_INET;
				WSAHtons(_IOCPSOCKET.socket, _IOCPSOCKET.port, &toAddr.sin_port);
				toAddr.sin_addr.S_un.S_addr = inet_addr(_IOCPSOCKET.IP);
				CZQString A = _IOCPSOCKET.IP;
				//关联完成端口
				_activeSocket->_socket = _IOCPSOCKET.socket;
				//_activeSocket->_remoteAddress = A;
				_activeSocket->_remotePort = _IOCPSOCKET.port;
				_activeSocket->_socketHandle = _IOCPSOCKET.socket;
				_activeSocket->_hIOCP = _hIOCP;

				if (CreateIoCompletionPort((HANDLE)_IOCPSOCKET.socket, _hIOCP, (ULONG_PTR)_activeSocket, 0) == 0)
				{
					_activeSocket->forceClose();
					_IOCPSOCKET.socket = INVALID_SOCKET;
					return result;
				}

				//连接SERVER
				int errorCode = 0;
				result = WSAConnect(_IOCPSOCKET.socket, (PSOCKADDR)&toAddr, sizeof(SOCKADDR_IN),
					nullptr, nullptr, nullptr, nullptr) == 0;

				if (!result)
				{
					errorCode = WSAGetLastError();
					result = errorCode == WSAEWOULDBLOCK;
				}//if

				if (!result)
				{
					_activeSocket->forceClose();
					_IOCPSOCKET.socket = INVALID_SOCKET;
					if (errorCode != 10061)
						_activeSocket->OnErrorEvent(errorCode);
				}
				else
				{
					if (OnConnectCallBack)
						OnConnectCallBack(_activeSocket);
					if (_IOCPSOCKET.socket != INVALID_SOCKET)
					{
						memset(&_activeSocket->_readBlock, 0, sizeof(Block));
						_activeSocket->prepareRecv(&_activeSocket->_readBlock);
					}
				}
			}//if
			else
				DWORD errorcode = GetLastError();
		}//if
		

	}//try
	catch (...)
	{
		DWORD errorcode = GetLastError();
	}
	return result;
}

void CZQ_ClientIOCPSocket::activestop()
{
	if (_activeSocket->_socket!=INVALID_SOCKET)
	{
		_activeSocket->forceClose();
		_IOCPSOCKET.socket = INVALID_SOCKET;
		if (OnDisConnectCallBack)
		{
			OnDisConnectCallBack(_activeSocket);
		}
	}//if
	if (_hIOCP != 0)
	{
		HANDLE iocphandle = _hIOCP;
		_hIOCP = 0;
		CloseHandle(iocphandle);
	}
}
