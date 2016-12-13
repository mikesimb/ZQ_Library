#pragma once
#include "ZQ_ClientIOCPSocket.h"
#include "ZQ_MessageNodeQueue.h"
#include "ZQString.h"
using namespace std;

class CZQ_IOCPClient :public CZQ_ClientIOCPSocket
{
public:
	CZQ_IOCPClient();
	virtual ~CZQ_IOCPClient();
public:
	virtual void init();
	virtual void onSocketError(CZQ_CustomIOCPClientSocket * client, int& errorCode);
	virtual void OnConnect(CZQ_CustomIOCPClientSocket* socket);
	virtual void OnDisConnect(CZQ_CustomIOCPClientSocket * socket);

	virtual void connect();
	virtual void connect(CZQString ip, int port);

	void SendMessage(MsgRef* msg);

	void SendBufData(void* buf, int buflen);

	virtual void update();

	//inline  MessageDispatcher* getMessageDispatcher() { return _msgDispathcer };
	inline bool isConnected() { return _isConnected; };

protected:
	virtual void onSocketRead(CZQ_CustomIOCPClientSocket * socket, void * buf, int count);
	void socketReadForMsgHead(void* buf, int count);
	void socketReadForServerMsgHead(void* buf, int count);
	virtual void reviceMsg(MsgRef * msg, int cmd);
	virtual void touchMsgQueue(MsgRef* msg);

	bool _isConnected = false;
	bool _isUseServerMsgHead = false;
	CZQString _ip = " ";

	int _port = 0;

	CZQ_MessageNodeQueue* _msgQueue = nullptr;

	//Messagedispathcer* _msgDispatcher = nullptr;

private:
	int _totalRecvDatalen = 0;

	int _dataNodeCount = 0;

	char * _reviceBuffer = nullptr;

	uint32_t _reviceLen = 0;

	ZQ_MessageQueueTouchProc _queueTouchProc = nullptr;

};