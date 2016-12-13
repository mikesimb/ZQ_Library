#include "stdafx.h"
#include "ZQ_IOCPClient.h"


CZQ_IOCPClient::CZQ_IOCPClient()
{
	WSAStartupSocket();
	//OnSocketError = IOCP_EVENT_CALLBACK_2(CZQ_IOCPClient::onSocketError this);
	OnConnectCallBack = IOCP_EVENT_CALLBACK_1(CZQ_IOCPClient::OnConnect, this);
	OnDisConnectCallBack = IOCP_EVENT_CALLBACK_1(CZQ_IOCPClient::OnDisConnect, this);
	setOnReadEvent(IOCP_EVENT_CALLBACK_3(CZQ_IOCPClient::onSocketRead, this));
	_msgQueue = new CZQ_MessageNodeQueue();
	//_msgDispatcher = new MessageDispatcher();

}


CZQ_IOCPClient::~CZQ_IOCPClient()
{
	close();
	_isConnected = false;
	if (_reviceBuffer)
	{
		free(_reviceBuffer);
		_reviceBuffer = nullptr;

	}

	//delete _msgDispatcher;
	//_msgDispatcher = nullptr;

	_queueTouchProc = nullptr;

	WSACleanupSocket();
}

void CZQ_IOCPClient::init()
{
	if (_isUseServerMsgHead)
		_reviceBuffer = (char*)malloc(SERVER_MAX_SEND_SIZE);
	else
		_reviceBuffer = (char*)malloc(CLIENT_MAX_RECEIVE_SIZE);
	_queueTouchProc = std::bind(&CZQ_IOCPClient::touchMsgQueue, this, std::placeholders::_1);

}

void CZQ_IOCPClient::onSocketError(CZQ_CustomIOCPClientSocket * client, int& errorCode)
{
	_isConnected = false;
	OutputDebugStringA("connect is Error");
}

void CZQ_IOCPClient::OnConnect(CZQ_CustomIOCPClientSocket* socket)
{
	_isConnected = true;
	OutputDebugStringA("connect is sceuss");
}

void CZQ_IOCPClient::OnDisConnect(CZQ_CustomIOCPClientSocket * socket)
{
	_isConnected = false;
	OutputDebugStringA("connect is disconnect");

}

void CZQ_IOCPClient::connect()
{
	if (!_isConnected && (_ip != "") && (_port > 0))
	{
		setServerIP(_ip);
		setServerPort(_port);
		open();
	}
}

void CZQ_IOCPClient::connect(CZQString ip, int port)
{
	if (!_isConnected && (ip != "") && (port > 0))
	{
		_ip = ip;
		_port = port;
		setServerIP(_ip);
		setServerPort(_port);
		open();
	}
}

void CZQ_IOCPClient::SendMessage(MsgRef* msg)
{
	if (_isConnected)
		this->getSocket()->sendBuf((char*)msg, msg->datalength);

}

void CZQ_IOCPClient::SendBufData(void* buf, int buflen)
{
	if (_isConnected && buflen > 0)
	{
		this->getSocket()->sendBuf((char*)buf, buflen);
	}
}

void CZQ_IOCPClient::update()
{
	if (_msgQueue && _queueTouchProc)
	{
		_msgQueue->touch(_queueTouchProc);
	}
}

void CZQ_IOCPClient::onSocketRead(CZQ_CustomIOCPClientSocket * socket, void * buf, int count)
{
	if (_isUseServerMsgHead)
		socketReadForServerMsgHead(buf, count);
	else
		socketReadForMsgHead(buf, count);
}

void CZQ_IOCPClient::socketReadForMsgHead(void* buf, int count)
{
	if (_reviceLen + count > CLIENT_MAX_RECEIVE_SIZE)
	{
		count = CLIENT_MAX_RECEIVE_SIZE - _reviceLen;

	}

	memcpy(&_reviceBuffer[_reviceLen], buf, count);

	_reviceLen = count;

	uint32_t offset = 0;
	bool bWarning = false;
	int flag = 0;
	while ((_reviceLen - offset) >= sizeof(MsgHead))
	{
		MsgHead* pMsg = (MsgHead*)(&_reviceBuffer[offset]);
		if (pMsg->sign = MSG_SIGN)
		{
			if (pMsg->datalength > _reviceLen)
			{
				offset = _reviceLen;
				flag = 1;
				bWarning = true;

			}

			if (pMsg->datalength < sizeof(MsgHead))
			{
				offset++;
			}
			else
			{
				if (offset + pMsg->datalength > _reviceLen)
					break;
				else
				{
					reviceMsg(pMsg, pMsg->cmd);
					offset = pMsg->datalength;
				}
				

			}
		}
		else
		{
			bWarning = true;
			offset++;
			flag = 2;

		}
	}// while;

	if (bWarning)
	{
		OutputDebugStringA("Reviced incomplete data package");

	}

	if (offset = _reviceLen)
	{
		_reviceLen = 0;
	}
	else
	{
		memcpy(&_reviceBuffer[0], &_reviceBuffer[offset], _reviceLen - offset);
		_reviceLen = _reviceLen - offset;

	}
}

void CZQ_IOCPClient::socketReadForServerMsgHead(void* buf, int count)
{
	if (_reviceLen + count > SERVER_MAX_SEND_SIZE)
	{
		count = SERVER_MAX_SEND_SIZE - _reviceLen;

	}
	memcpy(&_reviceBuffer[_reviceLen], buf, count);

	_reviceLen += count;

	uint32_t offset = 0;
	bool bWarning = false;

	int flag = 0;

	while ((_reviceLen - offset) >= sizeof(ServerMsgHead))
	{
		ServerMsgHead * pMsg = (ServerMsgHead*)(&_reviceBuffer[offset]);
		if (pMsg->sign == SERVER_MSG_SIGN)
		{
			if (pMsg->datalength > _reviceLen)
			{
				offset = _reviceLen;
				flag = 1;
				bWarning = true;
			}//if

			if (pMsg->datalength < sizeof(ServerMsgHead))
			{
				offset++;
			}//if
			else
			{
				if (offset + pMsg->datalength > _reviceLen)
					break;
				else
				{
					reviceMsg(pMsg, pMsg->cmd);
					offset += pMsg->datalength;
				}//if
			}//if
		}//if
		else
		{
			bWarning = true;
			offset++;
			flag = 2;

		}
	}//while

	if (bWarning)
	{
		OutputDebugStringA("Reviced incomplete data package");
	}
	if (offset == _reviceLen)
	{
		_reviceLen = 0;
	}
	else
	{
		memcpy(&_reviceBuffer[0], &_reviceBuffer[offset], _reviceLen - offset);
		_reviceLen = _reviceLen - offset;
	}
}

void CZQ_IOCPClient::reviceMsg(MsgRef * msg, int cmd)
{
	if (_msgQueue)
		_msgQueue->addNode(msg);
	else
		OutputDebugStringA("reviced unprocessMsg data cmd ");
}

void CZQ_IOCPClient::touchMsgQueue(MsgRef* msg)
{
// 	if (_msgDispathcer)
// 	{
// 		if (_isUseServerMsgHead)
// 		{
// 			_msgDispathcer->dispatchEvent(static_cast<ServerMsgHead*>(msg)->cmd, msg);
// 
// 		}
// 		else
// 			_msgDispathcer->dispatchEvent(static_cast<MsgHead*>(msg)->cmd, msg);
// 	}
}

