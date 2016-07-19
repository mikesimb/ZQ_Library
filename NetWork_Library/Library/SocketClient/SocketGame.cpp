// ------------------------------------------------------------------------- 
//	文件名	: 	kSocketGame.cpp
//	创建者	:	方煜宽
//	创建时间:	2011-12-8 16:13
//	功能描述:	游戏客户端Socket
//
// -------------------------------------------------------------------------
#include "stdafx.h"
#include "SocketGame.h"
#include "../Utils/ErrorCheck.h"
#include "Packet.h"

CSocketGame::CSocketGame()
{
	
}

CSocketGame::~CSocketGame()
{
	m_SocketFuncMap.clear();
}

BOOL CSocketGame::RegFunc(WORD wID, const MemberFuncInfo& info)
{
	SocketFuncMap::iterator itr = m_SocketFuncMap.find(wID);
	if (itr != m_SocketFuncMap.end())
	{
		itr->second = info;
	}
	else
	{
		m_SocketFuncMap.insert(make_pair(wID, info));

	}
	return TRUE;
}

BOOL CSocketGame::CheckSocketError(LPARAM lParam)
{
	if(!CSocket::CheckSocketError(lParam))
		return FALSE;

	return TRUE;
}

void CSocketGame::OnConnect(SOCKET host)
{
	
}

void CSocketGame::OnClose(SOCKET host)
{
	CSocket::OnClose(host);
	//::MessageBoxW(m_hWnd, L"Server connection closed", L"Network error", MB_ICONERROR);
}

/*
	查找【开始标志】位置
	返回-1表示没找到【开始标志】
*/
int CSocketGame::FindStarPos(char* pBuf, int nLen)
{
	for (int i = 1; i < nLen; i++)
	{
		pBuf++;
		if (SOCKETPACKET_START_SYMBOL == *(DWORD*)pBuf)
		{
			return i;
		}
	}
	return -1;
}

/*
	查找【开始标志】位置.
	ppBuf：新位置指针。
	nRemainLen：新位置后续长度，
	返回:原先*pBuf指针	
*/
char* CSocketGame::FindDataBegin(char** ppBuf, int& nLen)
{
	if (nLen <= 0)
		return *ppBuf;

	char* pOld = *ppBuf;

	DWORD dwValidate = *(DWORD*)*ppBuf;
	if (SOCKETPACKET_START_SYMBOL != dwValidate)
	{
		LogW::Trace(L"查找【开始标志】位置FindStarPos(char* pBuf, int nLen)");
		int nPos = FindStarPos(*ppBuf, nLen);
		if (-1 != nPos)
		{
			LogW::Trace(L"查找到的位置:%d", nPos);
			*ppBuf += nPos;
			nLen -= nPos;
		}
		else
		{
			LogW::Trace(L"没有查找到【开始标志】");
			*ppBuf += nLen;
			nLen = 0;
		}
	}
	return pOld;
}

void CSocketGame::ProcessSocketpacket()
{
	CSocketGame::Update();

	//while (m_RecvBuffer.Count() >= sizeof(Head))
	//{
	//	BYTE *pData = m_RecvBuffer.Buffer() + m_RecvBuffer.ReadPos(); 
	//	Head *pHead = (Head *)pData;
	//	assert(pHead->dwStartFlag == SOCKETPACKET_START_SYMBOL);
	//	if (m_RecvBuffer.Count() >= pHead->wPackageLen)
	//	{
	//		if (0 != (pHead->wIdent & COMPRESS_FLAG))
	//		{
	//			
	//		}
	//		else
	//			Dispatch(pHead->wIdent, (char *)(pData + sizeof(Head)), pHead->wPackageLen -sizeof(Head));
	//		m_RecvBuffer.Pop(pHead->wPackageLen);
	//	}
	//	else
	//		return;
	//}

	if (0 != m_recvBuf.Length())
	{
		char* pNext = ProcessSocketpacket(m_recvBuf.Buffer(), m_recvBuf.Length());
		if (pNext)
		{
			m_recvBuf.Remove(pNext - m_recvBuf.Buffer());
		}
		else
		{
			m_recvBuf.ClearAll();
		}
	}
}

char* CSocketGame::ProcessSocketpacket(char* pBuf, int nLen)
{
	if (nLen < sizeof(Head))
		return pBuf;

	const int HEAD_LEN = sizeof(Head);
	int nBufLen = nLen;

	// 查找正确开始位置
	char* pRet = pBuf;
	pRet = FindDataBegin(&pBuf, nBufLen);
	if (nBufLen <= HEAD_LEN)
		return pRet;

	Head head;
	memcpy(&head, pBuf, sizeof(Head));

	WORD wDataLen = head.wPackageLen - HEAD_LEN;
	while (head.wPackageLen <= nBufLen)
	{
		pBuf += HEAD_LEN;
		nBufLen -= HEAD_LEN;
		
		ProcessOneSocketpacket(head.wIdent, pBuf, wDataLen);

		pBuf += wDataLen;
		nBufLen -= (int)wDataLen;

		L_ASSERT(nBufLen >= 0);

		// 缓存区没有数据了
		if (0 == nBufLen)
			return pBuf;

		// 查找正确开始位置
		pRet = FindDataBegin(&pBuf, nBufLen);
		if (nBufLen <= HEAD_LEN)
			return pRet;

		memcpy(&head, pBuf, sizeof(Head));
		wDataLen = head.wPackageLen - HEAD_LEN;
	}
	return pRet;
}

void CSocketGame::ProcessOneSocketpacket(WORD wIdent, char* pBuf, int nLen)
{
	WORD wDataLen = nLen;
	char* des = NULL;
	if (wDataLen > 0)
	{
		if (HAS_FLAG(wIdent, COMPRESS_FLAG))
		{
			wIdent = wIdent & !COMPRESS_FLAG;

			DWORD dwUnCompressLen = 0;
			memcpy(&dwUnCompressLen, pBuf, sizeof(WORD));
			DWORD dwCompressLen = wDataLen - sizeof(WORD);

			wDataLen = (WORD)dwUnCompressLen;
			des = new char[wDataLen];
			Utils::UnCompress((BYTE*)des, &dwUnCompressLen, (BYTE*)pBuf, dwCompressLen);
		}
		else
		{
			des = new char[wDataLen];
			memcpy(des, pBuf, wDataLen);
		}
	}

	Dispatch(wIdent, des, wDataLen);
	SAFE_DELETE(des);
}

namespace
{
/*
调用类成员函数

bThisCall	: 成员函数调用约定类型, TRTE:hiscall,FALSE:其它类型
dwFuncAddr	: 成员函数地址
pThis		: 类对象的地址
nCount		: 成员函数参数个数
...			: 成员函数的参数列表
*/
DWORD CallMemberFunc(BOOL bThisCall, DWORD dwFuncAddr, void *pThis, int nCount,...)
{
	DWORD dwRet;

	if(nCount > 0)				// 有参数,将参数压入栈
	{
		__asm
		{
			mov  ecx, nCount;	// 参数个数,ecx,循环计数器.
			mov  edx, ecx;
			shl  edx, 2;
			add  edx, 0x14;  edx = nCount * 4 + 0x14;

next:	push  dword ptr[ebp + edx];
			sub   edx,0x4;
			dec   ecx
			jnz   next;
		}
	}

	// 处理this指针(__thiscall,vc默认的成员函数调用类型)
	if(bThisCall)
	{
		__asm mov ecx, pThis;
	}
	else // __stdcall
	{
		__asm push pThis;
	}

	__asm // 调用函数
	{
		call dwFuncAddr;
		mov  dwRet, eax;
	}

	return dwRet;
}

}

BOOL CSocketGame::Dispatch(WORD wID, char* pBuf, int nLen)
{
	SocketFuncMap::iterator itr = m_SocketFuncMap.find(wID);
	if (itr != m_SocketFuncMap.end())
	{
		DWORD This = (itr->second).dwThis;
		
		FUNCTYPE fnFunPtr  = (FUNCTYPE)(itr->second).dwFuncAddr;

		__asm
		{
			push ecx;
			mov ecx, This;
		}

		try
		{
			if (fnFunPtr)
			{
				CallMemberFunc(TRUE, (itr->second).dwFuncAddr, (void*)This, 2, pBuf, nLen);
				//fnFunPtr(pBuf, nLen);
			}

		}
		catch (...)
		{
			L_LOG_STR("Dispatch fnFunPtr Error");
		}

		__asm
		{
			pop ecx;
		}

		return TRUE;
	}
	return FALSE;
}

/*
							(Head)											     (Data)
	/-------------------------------------------------------------------\    /-------------\
	|网络包开始标识(4字节)|包的总长度(2字节)|协议号(2字节)|自增的包序号(4字节)|   |  没压缩的数据	|


	如果Ident>=0x8000 表示此封包为压缩数据，需要去掉高位得到正确的Ident				

							(Head)													   (Data)
	/-------------------------------------------------------------------\   /----------------------------\
	|网络包开始标识(4字节)|包的总长度(2字节)|协议号(2字节)|自增的包序号(4字节)|   | 原始长度(2字节) | 压缩的数据 |

*/
BOOL CSocketGame::SendData(WORD wID, LPCSTR pData /*= NULL*/, int nDataLen /*= 0*/)
{
#ifndef _NETWORK
	return TRUE;
#endif

	// Data
	WORD wDataLen = (WORD)nDataLen;
	if (pData)
	{
		if (HAS_FLAG(wID, 0x8000))
		{
			DWORD dwDataCompressLen = Utils::GetCompressLen(nDataLen);
			char* pDataBuf = m_SendData.GetDataBuf();
			memcpy(pDataBuf, &nDataLen, sizeof(WORD));

			char* pComDataBuf = m_SendData.GetComDataBuf();
			Utils::Compress((BYTE*)pComDataBuf, &dwDataCompressLen, (BYTE*)pData, (DWORD)nDataLen);

			wDataLen = sizeof(WORD) + (WORD)dwDataCompressLen;
		}
		else
		{
			char* pDataBuf = m_SendData.GetDataBuf();
			memcpy(pDataBuf, pData, nDataLen);
		}
	}

	if (wDataLen + sizeof(Head) > PACKET_BUF_LEN)
	{
		L_LOG_STR("包长度超过64K");
		return FALSE;
	}

	// Head
	static DWORD dwAutoID = (DWORD)rand();
	Head head;
	head.dwStartFlag = SOCKETPACKET_START_SYMBOL;
	head.wPackageLen = sizeof(Head) + wDataLen;
	head.wIdent = wID;
	head.dwAutoID = dwAutoID++;

	Head* pHeadBuf = (Head*)m_SendData.GetHeadBuf();
	memcpy(pHeadBuf, &head, sizeof(Head));

	Send(m_SendData.GetBuf(), head.wPackageLen);
	return TRUE;
}