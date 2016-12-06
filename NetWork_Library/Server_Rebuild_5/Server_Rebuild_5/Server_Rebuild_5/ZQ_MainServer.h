#pragma once
#include "IOCPTypeDef.h"
#include "ZQ_CustomIOCPServerSocket.h"


class CZQ_MainServer:public CZQ_CustomIOCPServerSocket
{
public:
	CZQ_MainServer();
	~CZQ_MainServer();
	static CZQ_MainServer* getInstance();
	
	
	virtual CZQ_CustomIOCPClientSocket* CreateNewClientSocket();
	virtual void ClientSocketError(CZQ_CustomIOCPClientSocket * clientsocket, int & errorcode);
	virtual void ClientSocketConnect(CZQ_CustomIOCPClientSocket* clientsocket);
	virtual void ClientSocketDisconnect(CZQ_CustomIOCPClientSocket* clientsocket);
	void StartServer(int port);
	void StopServer();
	virtual void update();
	inline bool isRequestListen() { return _isRequestListen; };
private:
	//是否已经启动监听
	bool _isRequestListen = false;
// 	static CZQ_MainServer* m_pInstance;



};

