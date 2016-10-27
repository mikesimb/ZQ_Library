#ifndef ZQ_CUSTOMIOCPSOCKET_H__
#define ZQ_CUSTOMIOCPSOCKET_H__
#pragma once

#include "IOCPTypeDef.h"
#include "ZQString.h"
class CZQ_CustomIOCPSocket
{
public:
	CZQ_CustomIOCPSocket();
	virtual ~CZQ_CustomIOCPSocket();

	//��������Ļص�����
	SocketErrorEvent  OnSocketError;
	//���ӳɹ�֪ͨ
	SocketNotifyEvent OnConnectCallBack;

	SocketNotifyEvent OnDisConnectCallBack;
	//���ӶϿ�֪ͨ
	SocketNotifyEvent OnSocketDisconnect;

	HANDLE   getIOCPHandle();
	IOCPSOCKET* getIOCPSOCKET();
	CZQString getServerIP();
	void setServerIP(CZQString IPAddress);
	int getServerPort();
	void setServerPort(const int Port);

	bool isAcriveed();
	void open();
	void close();

	virtual void setActive(bool bActive) = 0;
	virtual void freeclient(CZQ_CustomIOCPClientSocket* client) = 0;
	virtual bool activestart() = 0;
	virtual void activestop() = 0;

	void socketErrorEvent(CZQ_CustomIOCPClientSocket* clientsocket, int errorCode);
protected:
	HANDLE  _hIOCP = nullptr;

	IOCPSOCKET  _IOCPSOCKET;


};
#endif // ZQ_CUSTOMIOCPSOCKET_H__

