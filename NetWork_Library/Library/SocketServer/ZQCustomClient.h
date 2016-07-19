#pragma once
#include <WinSock2.h>
#include "ZQNetCommon.h"

class CZQCustomClient
{
public:
	CZQCustomClient();
	virtual ~CZQCustomClient();


	void SetAcitveTick(DWORD dwTick);
	void DoRevice(pBlock data, int buflen);
	void DoSend(pBlock data, int buflen);
	void ReadyReviceNextData(pBlock data);
	void ReadySendNextData(pBlock data, int buflen);
	//�����Ϊ���ܹ����Ժ�������ɵĴ���SOCKETRead��Ϣ
	virtual void SocketRead(pBlock data, int buflen);

	void SendBuffer(pBlock block);
	void SendBuffer(char * buffer, int buflen);
	void ForceClose();
	//�ر�����
	void Close();
	//���Ͷ��м�������
	void AddNode(pBlock data);
	void DeleteBeginNode();
	void ClearNodes();
	SOCKET  getsocket(){ return m_socket; };


protected:
	SOCKET    m_socket;
	char         m_address[16];
	int            m_Port;
	HANDLE  m_IOCP;
	Block        m_ReviceData;
	Block        m_SendData;
	//��־����Ϊ�Ƕ��̷߳��ͣ����ԣ���Ҫ�и�״̬�����ơ�
	bool         m_IsSending;
	//�±��Ƿ��Ͷ��е�����
	pClientSendData   m_beginpSendData;
	pClientSendData   m_endSendData;
	int          m_SendNodeCount;

	DWORD m_dwActiveTick;

	CRITICAL_SECTION  m_SendCS;
};

