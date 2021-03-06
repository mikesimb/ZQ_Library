#pragma once
#ifndef __ZQ_CLIENTIOCPSOCKET_H__
#define  __ZQ_CLIENTIOCPSOCKET_H__
#include "IOCPTypeDef.h"
#include "ZQ_CustomIOCPClientSocket.h"
#include "ZQ_CustomIOCPSocket.h"
#include "ZQ_AcceptThread.h"
class CZQ_ClientIOCPSocket:public CZQ_CustomIOCPSocket
{
public:
	CZQ_ClientIOCPSocket();
	virtual ~CZQ_ClientIOCPSocket();
	CZQ_CustomIOCPClientSocket * getSocket();
	void setOnReadEvent(SocketReviceEvent readEvent);
	virtual void freeclient(CZQ_CustomIOCPClientSocket* clientSocket) ;
	virtual bool activestart();
	virtual void activestop();
private:
	CZQ_CustomIOCPClientSocket * _activeSocket;
	CZQ_WorkThread* _workThread;
	virtual void setActive(bool value);
	
	
};
#endif

