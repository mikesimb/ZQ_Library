#include "stdafx.h"
#include "ZQClient.h"
#include <WS2tcpip.h>

#include "ZQNetCommon.h"
#include "Protocol.h"


CZQClient::CZQClient()
{
	WSADATA         wsd;            //WSADATA����  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		//��ʼ��ʧ�ܡ�
		return;
	}
}


CZQClient::~CZQClient()
{
	WSACleanup();       //�ͷ��׽�����Դ  
}

void CZQClient::Initialize()
{
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost)
	{
		//cout << "socket failed!" << endl;
		WSACleanup();//�ͷ��׽�����Դ  
		return  ;
	}

}

void CZQClient::UnInitialize()
{
	
	if (INVALID_SOCKET == sHost)
	{
		//cout << "socket failed!" << endl;
		closesocket(sHost);
		return;
	}
}

void CZQClient::Connect()
{
	servAddr.sin_family = AF_INET;
	inet_pton(servAddr.sin_family, "127.0.0.1", (void *)&servAddr.sin_addr.s_addr);
	//servAddr.sin_addr.s_addr = inet_pton(IPPROTO_TCP,"127.0.0.1");//inet_addr("127.0.0.1");
	servAddr.sin_port = htons((short)7100);
	int nServAddlen = sizeof(servAddr);

	//���ӷ�����  
	retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	if (SOCKET_ERROR == retVal)
	{
		//cout << "connect failed!" << endl;
		DWORD dw = GetLastError();
		OutputDebugString("����ʧ�ܣ�����");
		closesocket(sHost); //�ر��׽���  
		WSACleanup();       //�ͷ��׽�����Դ  
		return;
	}
	else
		OutputDebugString("���ӳɹ�������");
}

void CZQClient::Disconnect()
{
	closesocket(sHost);
}

void CZQClient::doWriteSocket()
{
	pS2CMsg msg = new S2CMsg;
	msg->MsgHeader = S2C_Msg_FLAG;
	msg->Reserved = 0;
	int len = sizeof(ServertoClientMsg);
	pServertoClient scMsg = new ServertoClientMsg;
	scMsg->MsgHandle = CM_Login;
	char * buf =(char *)malloc(len + sizeof(S2CMsg));;
	char * buf1 =buf;
	memcpy_s(buf1, sizeof(S2CMsg), msg, sizeof(S2CMsg));
	buf1 = buf1 + sizeof(S2CMsg);
	memcpy_s(buf1, sizeof(ServertoClientMsg), scMsg, sizeof(ServertoClientMsg));

	//send(this->sHost, "bbbbbbb",10, 0);
	send(this->sHost, (char*)buf, len+sizeof(S2CMsg), 0);
	buf1 = NULL;
	free(buf);
	delete scMsg;
	delete msg;
}


void CZQClient::doReadSocket()
{

}

char* CZQClient::MakeSocketMsg(char* buf,int buflen)
{
	return NULL;
}
