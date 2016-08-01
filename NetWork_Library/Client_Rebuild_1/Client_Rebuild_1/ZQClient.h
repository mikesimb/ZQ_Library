#pragma once

const int BUF_SIZE = 64;
class CZQClient
{
public:
	CZQClient();
	~CZQClient();
	//��ʼ��SOCKET

	void Initialize();
	void UnInitialize();
	//��һ�����Ӳ���
	void Connect();

	//�Ͽ�����
	void Disconnect();

	//��ȡ��������
	void  doReadSocket();

	char* MakeSocketMsg(char* buf, int buflen);


	//д����������
	void doWriteSocket();
private:
	SOCKET          sHost;          //�������׽���  
	SOCKADDR_IN     servAddr;       //��������ַ  
	char            buf[BUF_SIZE];  //�������ݻ�����  
	char            bufRecv[BUF_SIZE];
	int             retVal;         //����ֵ  

};

