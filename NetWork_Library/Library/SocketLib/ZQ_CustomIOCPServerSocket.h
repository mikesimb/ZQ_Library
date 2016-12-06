#ifndef _ZQ_CustomIOCPServerSocket__H_  
#define _ZQ_CustomIOCPServerSocket__H_   
#include "IOCPTypeDef.h"
#include "ZQ_CustomIOCPClientSocket.h"
#include "ZQ_CustomIOCPSocket.h"
#include "ZQ_ClientSocketPool.h"
#include "ZQ_AcceptThread.h"
#include <vector>
#include "ZQString.h"
class CZQ_CustomIOCPServerSocket:public  CZQ_CustomIOCPSocket
{
public:
	CZQ_CustomIOCPServerSocket();
	~CZQ_CustomIOCPServerSocket();

public:
	SocketReviceEvent OnSocketRevice;
	OnCheckValidEvent OnCheckIPAddress;
	CreateCustomSocketEvent OnCreateSocketEvent;

	void touch(ClientSocketTouchProc touchProc, uint32_t param);
	void close();
	std::vector<CZQ_CustomIOCPClientSocket*>* getActiveConnects();
	int getCount();
	int hashCount();
	int poolCount();

	bool isValidAddress(CZQString ip, int nIP, int iport);
	void socketAccept(SOCKET socket, char* IP, int port);

	inline virtual void startListenSuccess(const IOCPSOCKET * iocpsocket) {};
	inline virtual void startListenFailed(int errCode) {};

protected:
	void lock();
	void unlock();
	CZQ_CustomIOCPClientSocket * valueof(const uint32_t key);

private:
	RTL_CRITICAL_SECTION  _client_cs;
	std::vector<CZQ_CustomIOCPClientSocket*> _activeConnectList;
	int _clientCount;
	int _hashClientCount;
	//这里是一个hash类应该可以考虑修改一下
	HashPortItem * _HandleBuckets[HASH_IPITEM_BUCKETS];

	CZQ_ClientSocketPool* _freeClientPool;
	uint64_t _timeOutCheckTick;
	//这里是一个接受线程
	CZQ_AcceptThread* _acceptThread;

	std::vector<CZQ_WorkThread*> _workThreads;
	uint32_t _currentHandle;

	pHashPortItem * portItemFind(uint32_t key);
	void portItemAdd(const uint32_t key, CZQ_CustomIOCPClientSocket * CustomIOCPClientSocket);
	void portItemRemote(const uint32_t key);
	void portItemClear();
	int getPoolCount();

	virtual void setActive(bool bActive);
	void freeCorpseClient();

	 virtual void freeclient(CZQ_CustomIOCPClientSocket * clientSocket);

	 virtual bool activestart();
	 virtual void activestop();

	bool disposeTimeOutClient(CZQ_CustomIOCPClientSocket * clientsocket, uint64_t param);
	bool disposeAllClient(CZQ_CustomIOCPClientSocket * clientSocket, uint64_t param);





	

};

#endif