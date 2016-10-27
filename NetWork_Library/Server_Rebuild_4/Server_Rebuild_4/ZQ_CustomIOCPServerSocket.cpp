#include "stdafx.h"
#include "ZQ_CustomIOCPServerSocket.h"
#include <WinSock2.h>
#include <functional>

using namespace std;


CZQ_CustomIOCPServerSocket::CZQ_CustomIOCPServerSocket()
	:_clientCount(0)
	,_hashClientCount(0)
	,_freeClientPool(nullptr)
	,_timeOutCheckTick(0)
	,_acceptThread(nullptr)
	,_currentHandle(1000)
	,OnSocketRevice(nullptr)
	,OnCheckIPAddress(nullptr)
	,OnCreateSocketEvent(nullptr)
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	int processorsCount = sysinfo.dwNumberOfProcessors;
	if (processorsCount >= MAX_WORK_THREAD_SIZE)
	{
		processorsCount = MAX_WORK_THREAD_SIZE - 1;
	}
	_workThreads.resize(processorsCount);

	for(auto iter = _workThreads.begin();iter != _workThreads.end(); ++iter)
	{
		auto t = *iter;
		t = nullptr;
	}

	_freeClientPool = new CZQ_ClientSocketPool();

	InitializeCriticalSection(&_client_cs);
	_timeOutCheckTick = GetTickCount64();

	for (int i = 0; i < HASH_IPITEM_BUCKETS; i++)
	{
		_HandleBuckets[i] = nullptr;
	}
}


CZQ_CustomIOCPServerSocket::~CZQ_CustomIOCPServerSocket()
{
	close();
	_freeClientPool->clear(std::bind(&CZQ_CustomIOCPServerSocket::disposeAllClient, 
									 this, 
									std::placeholders::_1, 
									std::placeholders::_2), MAX_FREE_TIMEOUT);
	delete _freeClientPool;
	_freeClientPool = nullptr;

	DeleteCriticalSection(&_client_cs);
	portItemClear();

	_activeConnectList.clear();

}

void CZQ_CustomIOCPServerSocket::touch(ClientSocketTouchProc touchProc, uint32_t param)
{

	EnterCriticalSection(&_client_cs);
	do {
		for (int i = _activeConnectList.size() - 1; i >= 0; i--)
		{
			do {
				auto iter = _activeConnectList.at(i);
				if (touchProc != nullptr)
				{
					touchProc(iter, param);
				}
			} while (0);
		}
	} while (0);
	LeaveCriticalSection(&_client_cs);
}

void CZQ_CustomIOCPServerSocket::close()
{
	setActive(false);
	EnterCriticalSection(&_client_cs);
	do {
		for (int i = _activeConnectList.size() - 1; i >= 0; i--)
		{
			auto clientsocket = _activeConnectList.at(i);
			freeclient(clientsocket);
			clientsocket = nullptr;
		}
		_freeClientPool->clear(std::bind(&CZQ_CustomIOCPServerSocket::disposeAllClient, this, std::placeholders::_1, std::placeholders::_2), MAX_FREE_TIMEOUT);

	} while (0);
	LeaveCriticalSection(&_client_cs);
}

std::vector<CZQ_CustomIOCPClientSocket*>* CZQ_CustomIOCPServerSocket::getActiveCount()
{
	return &_activeConnectList;
}

int CZQ_CustomIOCPServerSocket::getCount()
{
	return _clientCount;
}

int CZQ_CustomIOCPServerSocket::hashCount()
{
	return _hashClientCount;
}

int CZQ_CustomIOCPServerSocket::poolCount()
{
	return _freeClientPool->getCount();
}

bool CZQ_CustomIOCPServerSocket::isValidAddress(CZQString ip, int nIP, int iport)
{
	bool result = true;
	if (_clientCount > MAX_CLIENT_COUNT)
	{
		result = false;
	}
	else if (OnCheckIPAddress && !OnCheckIPAddress(ip))
	{
		result = false;
	}
	return result;

}

void CZQ_CustomIOCPServerSocket::socketAccept(SOCKET socket, char* IP, int port)
{
	EnterCriticalSection(&_client_cs);
	do {
		freeCorpseClient();
		auto clientsocket = _freeClientPool->deQueue();
		if (clientsocket == nullptr)
		{
			if (OnCreateSocketEvent != nullptr)
				clientsocket = OnCreateSocketEvent();
			else
				clientsocket = new CZQ_CustomIOCPClientSocket();
		}
		if (clientsocket == nullptr) return;

		clientsocket->_socketHandle = _currentHandle;
		clientsocket->_remoteAddress = IP;
		clientsocket->_remotePort = port;
		clientsocket->_socket = socket;
		clientsocket->_hIOCP = _hIOCP;
		clientsocket->_onSocketErrorEvent = std::bind(&CZQ_CustomIOCPSocket::socketErrorEvent, this, std::placeholders::_1, std::placeholders::_2);
		clientsocket->initialize();
		_activeConnectList.push_back(clientsocket);
		_clientCount++;

		portItemAdd(_currentHandle, clientsocket);

		if (clientsocket->_socket != INVALID_SOCKET)
		{
			if (CreateIoCompletionPort((HANDLE)socket, _hIOCP, (unsigned long)clientsocket, 0) == 0)
			{
				socketErrorEvent(clientsocket, GetLastError());
				clientsocket->forceClose();
			}
			else
			{
				if (OnConnectCallBack)
					OnConnectCallBack(clientsocket);
				if (clientsocket->_socket != INVALID_SOCKET)
					clientsocket->prepareRecv(&clientsocket->_readBlock);

			}
		}
		pHashPortItem p = nullptr;
		do 
		{
			_currentHandle++;
			if (_currentHandle < 1000)
				_currentHandle = 1000;
			p = *(portItemFind(_currentHandle));

		} while (p != nullptr);
	} while (0);
	LeaveCriticalSection(&_client_cs);
}

void CZQ_CustomIOCPServerSocket::lock()
{
	EnterCriticalSection(&_client_cs);
}

void CZQ_CustomIOCPServerSocket::unlock()
{
	LeaveCriticalSection(&_client_cs);
}

CZQ_CustomIOCPClientSocket * CZQ_CustomIOCPServerSocket::valueof(const uint32_t key)
{
	CZQ_CustomIOCPClientSocket  * result = nullptr;
	pHashPortItem item = *(portItemFind(key));
	if (item != nullptr)
	{
		result = item->CustomSocket;
	}
	return result;
}

pHashPortItem * CZQ_CustomIOCPServerSocket::portItemFind(uint32_t key)
{
	int hash = key % HASH_IPITEM_BUCKETS;
	pHashPortItem* result = &(_HandleBuckets[hash]);
	while ((*result) != nullptr)
	{
		if ((*result)->SocketHandle == key)
		{
			break;
		}
		else
			result = &((*result)->next);
	}
	return result;
}

void CZQ_CustomIOCPServerSocket::portItemAdd(const uint32_t key, CZQ_CustomIOCPClientSocket * CustomIOCPClientSocket)
{
	int hash = key % HASH_IPITEM_BUCKETS;
	pHashPortItem p = new  HashPortItem;
	p->SocketHandle = key;
	p->CustomSocket = CustomIOCPClientSocket;
	p->next = _HandleBuckets[hash];
	_HandleBuckets[hash] = p;
	_hashClientCount++;
}

void CZQ_CustomIOCPServerSocket::portItemRemote(const uint32_t key)
{
	auto prev = portItemFind(key);
	pHashPortItem p = *prev;
	if (p != nullptr)
	{
		(*prev) = p->next;
		delete p;

	}
	_hashClientCount--;

}

void CZQ_CustomIOCPServerSocket::portItemClear()
{
	pHashPortItem p = nullptr;
	pHashPortItem n  = nullptr;

	for (int i = 0; i < HASH_IPITEM_BUCKETS; i++)
	{
		p = _HandleBuckets[i];
		while (p != nullptr)
		{
			n = p->next;
			delete p;
			p = n;

		}
		_HandleBuckets[i] = nullptr;

	}
	_hashClientCount = 0;

}

int CZQ_CustomIOCPServerSocket::getPoolCount()
{
	if (_freeClientPool)
		return _freeClientPool->getCount();
	else
		return 0;

}

void CZQ_CustomIOCPServerSocket::setActive(bool bActive)
{
	if (bActive)
	{
		if(_IOCPSOCKET.IP ==""|| _IOCPSOCKET.port == 0 || _IOCPSOCKET.socket != INVALID_SOCKET)
		{
			return;
		}
		for (int i = _workThreads.size() -1; i > 0; i--)
		{
			auto t = _workThreads[i];
			if (t != nullptr)
			{
				delete t;
				_workThreads[i] = nullptr;

			}
		}
		_workThreads[0] = new CZQ_WorkThread(this, true);

	}
	else
	{
		PostQueuedCompletionStatus(_hIOCP, 0, 0, (LPOVERLAPPED)SHUTDOWN_FLAG);
		for (int i = _workThreads.size() - 1; i >= 0; i--)
		{
			auto t = _workThreads[i];
			if (t != nullptr)
			{
				delete t;
				_workThreads[i] = nullptr;

			}
		}
	}
}

void CZQ_CustomIOCPServerSocket::freeCorpseClient()
{
	uint64_t  tick = GetTickCount();
	uint64_t   dtick = tick - _timeOutCheckTick;
	if (tick > MAX_CHECK_TIMEOUT)
	{
		_timeOutCheckTick = tick;
		tick = dtick / 100;
		if (_freeClientPool)
		{
			_freeClientPool->clear(std::bind(&CZQ_CustomIOCPServerSocket::disposeTimeOutClient, this, std::placeholders::_1, std::placeholders::_2), tick);

		}

		for (int i = _activeConnectList.size() - 1; i > 0; i--)
		{
			auto pClient = _activeConnectList[i];
			pClient->_currentTick += tick;
			if (pClient->getActiveTick() == 0)
			{
				pClient->setActiveTicket(pClient->_currentTick);
			}//if
			else
				if (pClient->_currentTick - pClient->getActiveTick() > MAX_ACTIVE_TIMEOUT)
				{
					pClient->setActiveTicket(pClient->_currentTick);
					pClient->forceClose();
			}//if
		}//for
	}//if

}

void CZQ_CustomIOCPServerSocket::FreeClient(CZQ_CustomIOCPClientSocket * clientSocket)
{
	EnterCriticalSection(&_client_cs);
	do{
		for (auto iter = _activeConnectList.begin(); iter != _activeConnectList.end(); iter++)
		{
			if ((*iter) == clientSocket)
			{
				_clientCount--;
				_freeClientPool->append(clientSocket);
				if (clientSocket->_socket != INVALID_SOCKET)
				{
					clientSocket->forceClose();
				}
				if (OnDisConnectCallBack != nullptr)
				{
					OnDisConnectCallBack(clientSocket);
				}
				clientSocket->finalize();
				_activeConnectList.erase(iter);
				break;
			}//if
		}//for
	} while (0);
	LeaveCriticalSection(&_client_cs);
}

bool CZQ_CustomIOCPServerSocket::activeStart()
{
	bool result = false;
	_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (_hIOCP != 0)
	{
		do {
			_IOCPSOCKET.socket = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_IP, nullptr, 0, WSA_FLAG_OVERLAPPED);
			if (_IOCPSOCKET.socket != INVALID_SOCKET)
			{
				do {
					SOCKADDR addr;
					memset(&addr, 0, sizeof(SOCKADDR));
					SOCKADDR_IN * addr_in = (SOCKADDR_IN*)(&addr);
					addr.sa_family = PF_INET;
					CZQString s = "0.0.0.0";
					if (s.Compare(_IOCPSOCKET.IP) == 0)
					{
						addr_in->sin_addr.S_un.S_addr = INADDR_ANY;

					}
					else
					{
						addr_in->sin_addr.S_un.S_addr = inet_addr(_IOCPSOCKET.IP);
					}
					WSAHtons(_IOCPSOCKET.socket, _IOCPSOCKET.port, &addr_in->sin_port);
					int ret = ::bind(_IOCPSOCKET.socket, &addr, sizeof(SOCKADDR));

					if (ret == 0)
					{
						_acceptThread = new CZQ_AcceptThread(this);
						for (size_t i = 1; i < _workThreads.size(); ++i)
						{
							_workThreads[i] = new CZQ_WorkThread(this, false);

						}
						result = true;
					}
					else
					{
						int err = WSAGetLastError();
						socketErrorEvent(nullptr, err);
						startListenFailed(err);
					}
				} while (0);
				if (!result)
				{
					closesocket(_IOCPSOCKET.socket);
					_IOCPSOCKET.socket = INVALID_SOCKET;
				}
				else
				{
					startListenSuccess(&_IOCPSOCKET);
				}
			}//if
			else
			{
				int err = GetLastError();
				startListenFailed(err);
			}
		} while (0);
		if (!result)
		{
			HANDLE iocpHandle = _hIOCP;
			_hIOCP = 0;
			CloseHandle(iocpHandle);
		}
	}
	else
		socketErrorEvent(nullptr, GetLastError());

	return result;
}

void CZQ_CustomIOCPServerSocket::activeStop()
{
	if (_acceptThread != nullptr)
	{
		delete _acceptThread;
		_acceptThread = nullptr;
	}

	for (int i = _activeConnectList.size() - 1; i >= 0; i--)
	{
		auto Clientsocket = _activeConnectList[i];
		Clientsocket->forceClose();
	}

	if (_hIOCP != 0)
	{
		HANDLE iocphandle = _hIOCP;
		_hIOCP = 0;
		CloseHandle(iocphandle);
	}
}

bool CZQ_CustomIOCPServerSocket::disposeTimeOutClient(CZQ_CustomIOCPClientSocket * clientsocket, uint64_t param)
{
	bool result = false;
	clientsocket->_currentTick += param;
	if (clientsocket->getActiveTick() == 0)
	{
		clientsocket->setActiveTicket(clientsocket->_currentTick);

	}else
		if (clientsocket->_currentTick - clientsocket->getActiveTick() > MAX_FREE_TIMEOUT)
		{
			delete clientsocket;
			clientsocket = nullptr;
			result = true;

		}
	return result;
}

bool CZQ_CustomIOCPServerSocket::disposeAllClient(CZQ_CustomIOCPClientSocket * clientSocket, uint64_t param)
{
	delete clientSocket;
	clientSocket = nullptr;
	return true;
}
