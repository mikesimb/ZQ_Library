#ifndef _ZQ_CUSTOMIOCPCLIENTSOCKET__H_
#define _ZQ_CUSTOMIOCPCLIENTSOCKET__H_
#include "ZQString.h"
#include "IOCPTypeDef.h"

using namespace std;

class CZQ_CustomIOCPClientSocket
{
public:
	CZQ_CustomIOCPClientSocket();
	~CZQ_CustomIOCPClientSocket();
	void close();
	int sendBuf(char* buf, int count, bool freeAfterSend = false);
	int sendText(CZQString s);

	CZQString getRemoteAddress();

	int getRemotePort();
	char* getData();
	int getSendBufferLen();
	uint32_t  getTotalSendCount();
	void  setTotalSendCount(uint32_t value);


	int forceClose();
	void setActiveTicket(uint64_t value);
	uint64_t getActiveTick();
	uint32_t getSocketHandle();

protected:
	void OnErrorEvent(int ErrorCode);
	virtual void initialize();
	virtual void finalize();
	virtual void socketRead(void *buf, int count);

private:
	void doClientRead(pBlock pRBlock, int transfered);
	void doClientSend(pBlock pSBlock, int transfered);
	void prepareRecv(pBlock pRBlock);
	void prepareSend(pBlock pSBlock,int iSendlen);
	void clearSendBuffer();

private:
	HANDLE _hIOCP;
	SOCKET  _socket;
	CZQString _remoteAddress;
	int _remotePort;
	char* _data;
	uint32_t  _socketHandle;
	Block    _sendBlock;
	Block    _readBlock;

	int     _totalBufferlen;
	RTL_CRITICAL_SECTION  _sendBufferCS;
	bool   _isSending;

	uint64_t   _currentTick;

	uint32_t  _totalSendDataCount;

	pSendQueueNode  _firstQueueNode;
	pSendQueueNode  _lastQueueNode;

	OnSocketErrorEvent _onSocketErrorEvent;
	
	SocketReviceEvent _onReviceEvent;

	uint64_t _uactiveTick;


	friend CZQ_WorkThread;
	friend CZQ_CustomIOCPServerSocket;


};
#endif // 

