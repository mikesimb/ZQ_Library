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

	//发生错误的回调函数
	SocketErrorEvent  OnSocketError;
	//连接成功通知
	SocketNotifyEvent OnConnectCallBack;

	SocketNotifyEvent OnDisConnectCallBack;
	//连接断开通知
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

