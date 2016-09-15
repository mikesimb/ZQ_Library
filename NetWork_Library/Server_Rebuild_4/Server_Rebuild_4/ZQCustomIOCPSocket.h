#pragma once
#include "IOCPTypeDef.h"
class ZQCustomIOCPSocket
{
public:
	ZQCustomIOCPSocket();
	~ZQCustomIOCPSocket();
	//SocketHandle��غ���
	SOCKET getSocketHandle();
	void setSocketHandle(SOCKET socket);
	//IP��ַ��غ���
	char * getIPAddress();
	void setIPAddress(char* ipaddress);
	//�˿���غ���
	int getPort();
	void setPort(int port);
	//��ȡ��ǰsocket��״̬
	bool isActived();
	virtual void setActived(bool actived) =0;

	void Open();

	void Close();

	virtual void freeClient(CZQ_CustomIOCPClientSocket * ClientSocket) = 0;

	virtual bool ActiveStart() = 0;

	virtual bool ActiveStop() = 0;

	void socketErrorEvent(CZQ_CustomIOCPClientSocket* ClientSocket, int ErrorCode);
private:
	//SOCKET���;
	SOCKET		m_socket;
	//IP��ַ
	char		m_address[16];
	//�˿ں�
	int         m_port;
	//��ɶ˿ں�
	HANDLE		m_hCompletionPort;
	//�Ƿ��ڼ���״̬
	bool m_isActived;
};

