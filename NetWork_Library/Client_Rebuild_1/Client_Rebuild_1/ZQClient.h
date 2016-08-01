#pragma once

const int BUF_SIZE = 64;
class CZQClient
{
public:
	CZQClient();
	~CZQClient();
	//初始化SOCKET

	void Initialize();
	void UnInitialize();
	//做一个连接操作
	void Connect();

	//断开连接
	void Disconnect();

	//读取网络数据
	void  doReadSocket();

	char* MakeSocketMsg(char* buf, int buflen);


	//写入网络数据
	void doWriteSocket();
private:
	SOCKET          sHost;          //服务器套接字  
	SOCKADDR_IN     servAddr;       //服务器地址  
	char            buf[BUF_SIZE];  //接收数据缓冲区  
	char            bufRecv[BUF_SIZE];
	int             retVal;         //返回值  

};

