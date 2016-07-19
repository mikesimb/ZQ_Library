// ------------------------------------------------------------------------- 
//	�ļ���	: 	Packet.h
//	������	:	���Ͽ�
//	����ʱ��:	2012-3-14 17:15
//	��������:	�����ID����
//	ע��	:   ����ID������棬��ϸ˵�����ṹ����
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
//#define CM_Run		2		// 	nRecog��Э��ID; wIdent:����x����; wParam������y����; wTag:��ɫID wSeries��δʹ��

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

// �����ð�
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