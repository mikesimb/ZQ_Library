#pragma once
#include "stdafx.h"
//本单元是协议的结构定义和命令定义
#define S2C_Msg_FLAG     0xFFFFFFE

#define CM_Login  1001;
#define SM_Login_OK 1002;
#define SM_Login_Failed 1003;

typedef struct tgaS2CMsg
{
	DWORD MsgHeader;
	DWORD Reserved;//保留关键字
	int	  BufLen;
}S2CMsg,*pS2CMsg;

typedef struct tgaServertoClientMsg
{
	DWORD Recog;
	WORD  MsgHandle;
	WORD  LParam1;
	WORD  WParam2;

}ServertoClientMsg,*pServertoClient;
