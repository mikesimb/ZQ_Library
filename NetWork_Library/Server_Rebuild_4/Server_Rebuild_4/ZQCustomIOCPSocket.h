#pragma once
#include "IOCPTypeDef.h"
class ZQCustomIOCPSocket
{
public:
	ZQCustomIOCPSocket();
	~ZQCustomIOCPSocket();
	//SocketHandle相关函数
	SOCKET getSocketHandle();
	void setSocketHandle(SOCKET socket);
	//IP地址相关函数
	char * getIPAddress();
	void setIPAddress(char* ipaddress);
	//端口相关函数
	int getPort();
	void setPort(int port);
	//获取当前socket的状态
	bool isActived();
	virtual void setActived(bool actived) =0;

	void Open();

	void Close();

	virtual void freeClient(CZQ_CustomIOCPClientSocket * ClientSocket) = 0;

	virtual bool ActiveStart() = 0;

	virtual bool ActiveStop() = 0;

	void socketErrorEvent(CZQ_CustomIOCPClientSocket* ClientSocket, int ErrorCode);
private:
	//SOCKET句柄;
	SOCKET		m_socket;
	//IP地址
	char		m_address[16];
	//端口号
	int         m_port;
	//完成端口好
	HANDLE		m_hCompletionPort;
	//是否处于激活状态
	bool m_isActived;
};

