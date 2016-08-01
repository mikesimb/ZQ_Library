#include "stdafx.h"
#include "ZQClient.h"
#include <WS2tcpip.h>

#include "ZQNetCommon.h"
#include "Protocol.h"


CZQClient::CZQClient()
{
	WSADATA         wsd;            //WSADATA变量  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		//初始化失败。
		return;
	}
}


CZQClient::~CZQClient()
{
	WSACleanup();       //释放套接字资源  
}

void CZQClient::Initialize()
{
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost)
	{
		//cout << "socket failed!" << endl;
		WSACleanup();//释放套接字资源  
		return  ;
	}

}

void CZQClient::Connect()
{
	servAddr.sin_family = AF_INET;
	inet_pton(servAddr.sin_family, "127.0.0.1", (void *)&servAddr.sin_addr.s_addr);
	//servAddr.sin_addr.s_addr = inet_pton(IPPROTO_TCP,"127.0.0.1");//inet_addr("127.0.0.1");
	servAddr.sin_port = htons((short)7100);
	int nServAddlen = sizeof(servAddr);

	//连接服务器  
	retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	if (SOCKET_ERROR == retVal)
	{
		//cout << "connect failed!" << endl;
		DWORD dw = GetLastError();
		OutputDebugString("连接失败！！！");
		closesocket(sHost); //关闭套接字  
		WSACleanup();       //释放套接字资源  
		return;
	}
	else
		OutputDebugString("连接成功！！！");
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

	send(this->sHost, (char*)scMsg, 10, 0);

}


void CZQClient::doReadSocket()
{

}

char* CZQClient::MakeSocketMsg(char* buf,int buflen)
{

}
