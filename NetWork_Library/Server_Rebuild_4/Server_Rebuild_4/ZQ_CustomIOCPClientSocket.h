#ifndef _ZQ_CUSTOMIOCPCLIENTSOCKET__H_
#define _ZQ_CUSTOMIOCPCLIENTSOCKET__H_
#include "ZQString.h"
#include "IOCPTypeDef.h"

using namespace std;

class CZQ_CustomIOCPClientSocket
{
public:
	CZQ_CustomIOCPClientSocket();
	~CZQ_CustomIOCPClientSocket();
	void close();
	int sendBuf(char* buf, int count, bool freeAfterSend = false);
	int sendText(CZQString s);

	CZQString getRemoteAddress();

	int getRemotePort();
	char* getData();
	int getSendBufferLen();
	uint32_t  getTotalSendCount();
	void  setTotalSendCount(uint32_t value);


	int forceClose();
	void setActiveTicket(uint64_t value);
	uint64_t getActiveTick();
	uint32_t getSocketHandle();

protected:
	void OnSocketEvent(int ErrorCode);

};
#endif // 

