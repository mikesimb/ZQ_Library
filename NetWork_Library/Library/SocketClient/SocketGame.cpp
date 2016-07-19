// ------------------------------------------------------------------------- 
//	�ļ���	: 	kSocketGame.cpp
//	������	:	���Ͽ�
//	����ʱ��:	2011-12-8 16:13
//	��������:	��Ϸ�ͻ���Socket
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
	���ҡ���ʼ��־��λ��
	����-1��ʾû�ҵ�����ʼ��־��
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
	���ҡ���ʼ��־��λ��.
	ppBuf����λ��ָ�롣
	nRemainLen����λ�ú������ȣ�
	����:ԭ��*pBufָ��	
*/
char* CSocketGame::FindDataBegin(char** ppBuf, int& nLen)
{
	if (nLen <= 0)
		return *ppBuf;

	char* pOld = *ppBuf;

	DWORD dwValidate = *(DWORD*)*ppBuf;
	if (SOCKETPACKET_START_SYMBOL != dwValidate)
	{
		LogW::Trace(L"���ҡ���ʼ��־��λ��FindStarPos(char* pBuf, int nLen)");
		int nPos = FindStarPos(*ppBuf, nLen);
		if (-1 != nPos)
		{
			LogW::Trace(L"���ҵ���λ��:%d", nPos);
			*ppBuf += nPos;
			nLen -= nPos;
		}
		else
		{
			LogW::Trace(L"û�в��ҵ�����ʼ��־��");
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

	// ������ȷ��ʼλ��
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

		// ������û��������
		if (0 == nBufLen)
			return pBuf;

		// ������ȷ��ʼλ��
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
�������Ա����

bThisCall	: ��Ա��������Լ������, TRTE:hiscall,FALSE:��������
dwFuncAddr	: ��Ա������ַ
pThis		: �����ĵ�ַ
nCount		: ��Ա������������
...			: ��Ա�����Ĳ����б�
*/
DWORD CallMemberFunc(BOOL bThisCall, DWORD dwFuncAddr, void *pThis, int nCount,...)
{
	DWORD dwRet;

	if(nCount > 0)				// �в���,������ѹ��ջ
	{
		__asm
		{
			mov  ecx, nCount;	// ��������,ecx,ѭ��������.
			mov  edx, ecx;
			shl  edx, 2;
			add  edx, 0x14;  edx = nCount * 4 + 0x14;

next:	push  dword ptr[ebp + edx];
			sub   edx,0x4;
			dec   ecx
			jnz   next;
		}
	}

	// ����thisָ��(__thiscall,vcĬ�ϵĳ�Ա������������)
	if(bThisCall)
	{
		__asm mov ecx, pThis;
	}
	else // __stdcall
	{
		__asm push pThis;
	}

	__asm // ���ú���
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
	|�������ʼ��ʶ(4�ֽ�)|�����ܳ���(2�ֽ�)|Э���(2�ֽ�)|�����İ����(4�ֽ�)|   |  ûѹ��������	|


	���Ident>=0x8000 ��ʾ�˷��Ϊѹ�����ݣ���Ҫȥ����λ�õ���ȷ��Ident				

							(Head)													   (Data)
	/-------------------------------------------------------------------\   /----------------------------\
	|�������ʼ��ʶ(4�ֽ�)|�����ܳ���(2�ֽ�)|Э���(2�ֽ�)|�����İ����(4�ֽ�)|   | ԭʼ����(2�ֽ�) | ѹ�������� |

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
		L_LOG_STR("�����ȳ���64K");
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