// ------------------------------------------------------------------------- 
//	文件名	: 	Packet.h
//	创建者	:	方煜宽
//	创建时间:	2012-3-14 17:15
//	功能描述:	网络包ID定义
//	注意	:   请在ID定义后面，详细说明包结构含义
//
// -------------------------------------------------------------------------
#ifndef __Packet_h__
#define __Packet_h__

#define CM_TEST 1000
#define SM_TEST 1000

#define CM_TEST1 1001
#define SM_TEST1 1001

//  Client To Serve Message
// -------------------------
//#define CM_LOGON	1
//#define CM_Run		2		// 	nRecog：协议ID; wIdent:攻击x坐标; wParam：攻击y坐标; wTag:角色ID wSeries：未使用

// for test
//#define SM_OK       WORD(1)
//#define SM_LOGON    WORD(2)
//#define SM_RUN      WORD(3)
//#define SM_WALK     WORD(4)
//#define SM_SHOW     WORD(5)
//#define SM_HIDE     WORD(6)
//#define SM_ATTACK   WORD(7)
//#define SM_TURN     WORD(8)
//
//#define SM_SPELL    WORD(9)
//#define SM_MAGIC    WORD(10)
//
//#define SM_Test		WORD(10000)

// Serve To Client Message
// ----------------------------
//#define SM_OK		1
//#define SM_RUN		2

// for test
//#define CM_LOGON    WORD(2)
//#define CM_RUN      WORD(3)
//#define CM_WALK     WORD(4)
//#define CM_SHOW     WORD(5)
//#define CM_HIDE     WORD(6)
//#define CM_ATTACK   WORD(7)
//#define CM_TURN     WORD(8)
//
//#define CM_SPELL    WORD(9)
//#define CM_MAGIC    WORD(10)
//#define CM_SHOOT1   WORD(11)
//#define CM_SHOOT2   WORD(12)
//
//#define CM_Test		WORD(10000)

#pragma pack(1)

// 测试用包
typedef struct tagTestPacket
{
	DWORD dwObjectID;
	WORD x;
	WORD y;
	BYTE Dir;
	DWORD dwValue;
	BYTE Flag;
}TestPacket, *LPTESTPACKET;

#pragma pack()

#endif // __packet_h__