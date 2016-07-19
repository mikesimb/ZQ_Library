// ------------------------------------------------------------------------- 
//	�ļ���	: 	kSocketGame.cpp
//	������	:	���Ͽ�
//	����ʱ��:	2011-12-8 16:13
//	��������:	��Ϸ�ͻ���Socket
//
// -------------------------------------------------------------------------
#include "stdafx.h"
#include "ClientSocket.h"
#include "../Utils/ErrorCheck.h"
#include "Packet.h"
#include "../Client/GTick.h"
#include "../Client/GPClient.h"

//////////////////////////////////////////////////////////////////////////
//CSocketBuffer
CSocketBuffer::CSocketBuffer():
m_pData(NULL), 
m_nReadPos(0), 
m_nWritePos(0), 
m_nCapacity(0)
{

}

CSocketBuffer::~CSocketBuffer()
{
	if (m_pData) 
		free(m_pData);
}

void CSocketBuffer::SetCapacity(INT nSize)
{
	if (nSize == 0)
		nSize = INIT_MSGQUEUE_CAPACITY;

	if (nSize > m_nCapacity)
	{
		m_pData = (CHAR *)realloc(m_pData, nSize);
		m_nCapacity = nSize;
	}
}

void CSocketBuffer::Clear()
{
	m_nWritePos = 0;
	m_nReadPos = 0;
}

BOOL CSocketBuffer::Append(CONST CHAR *pData, INT nSize)
{
	if (nSize > 0 && pData)
	{
		while (m_nCapacity < nSize + m_nWritePos)
			SetCapacity(m_nCapacity * 2);

		CHAR *pWrite = m_pData + m_nWritePos;
		memcpy(pWrite, pData, nSize);
		m_nWritePos += nSize;
		
		return TRUE;
	}
	return FALSE;
}

BOOL CSocketBuffer::Get(CHAR **ppData, INT &nSize)
{
	BOOL bRet = FALSE;
	if (ppData && m_nWritePos - m_nReadPos >= sizeof(Head))
	{
		INT nReadPos	= m_nReadPos;
		CHAR *pRead		= NULL;
		Head *pHead		= NULL;
		
		while (m_nWritePos - nReadPos >= sizeof(Head)) 
		{
			pRead = m_pData + nReadPos;
			DWORD dwSymbol = *(DWORD *)pRead;
			
			if (dwSymbol == SOCKETPACKET_START_SYMBOL)
			{
				pHead = (Head *)pRead;
				if (m_nWritePos - nReadPos >= pHead->wPackageLen)
				{
					*ppData = new CHAR[pHead->wPackageLen];
					nSize = pHead->wPackageLen;
					memcpy(*ppData, pRead, pHead->wPackageLen);
					pHead = (Head *)*ppData;

					nReadPos += pHead->wPackageLen;
					m_nReadPos = nReadPos;					
					
					bRet = TRUE;
				}
				break;
			}
			else
			{
				L_LOG_STR("invalid packet head-symbol");
				L_ASSERT(FALSE);
				nReadPos += sizeof(DWORD);
			}			
		}

		if (bRet && m_nReadPos == m_nWritePos)
		{
			m_nWritePos = 0;
			m_nReadPos = 0;
		}
	}
	return bRet;
}

#ifdef _GPROTECT
//////////////////////////////////////////////////////////////////////////
//CGPCheckThread
CGPCheckThread::CGPCheckThread():
m_pBuf(NULL),
m_nLen(0)
{
	InitializeCriticalSection(&m_csLock);
	m_hThread = (HANDLE)_beginthread(&agent, 0, this);
}

CGPCheckThread::~CGPCheckThread()
{
	m_Terminated = TRUE;
	TerminateThread(m_hThread, 0);
	DeleteCriticalSection(&m_csLock);
}

void CGPCheckThread::agent(void *p)
{
	((CGPCheckThread *)p)->ThreadExecute();
}

void CGPCheckThread::ThreadExecute()
{
	SetThreadAffinityMask(GetCurrentThread(), 0xFD);

	while (!m_Terminated)
	{
		if (m_pBuf)
		{
			EnterCriticalSection(&m_csLock);

			#if (defined(_DEBUG) || defined(INTERIOR))
				CDebugTick DebugTick;
				DebugTick.Start();
			#endif //_DEBUG

			g_pGProtect->ProcessResponse((unsigned char *)m_pBuf, m_nLen);
			g_socket.SendData(CM_GPS_CHECK_RESPONSE, m_pBuf, m_nLen);
			SAFE_DELETE_ARRAY(m_pBuf);

			#if (defined(_DEBUG) || defined(INTERIOR))
				DebugTick.End();
				OutputDebugStringW(Utils::FormatWStr(L"GPCheck %d \n", DebugTick.GetTick()).c_str());
			#endif //_DEBUG

			LeaveCriticalSection(&m_csLock);
		}
		else
		{
			Sleep(10);
		}
	}
	_endthread();
}

void CGPCheckThread::SetCheckData(CHAR* pBuf, INT nLen)
{
	EnterCriticalSection(&m_csLock);
	m_pBuf = new CHAR[nLen];
	m_nLen = nLen;
	memcpy(m_pBuf, pBuf, nLen);
	LeaveCriticalSection(&m_csLock);
}
#endif //_GPROTECT

//////////////////////////////////////////////////////////////////////////
//CClientSocket
CClientSocket::CClientSocket(CClientSocketCallBackInterface *pCallBackIntf/* = NULL*/):
CTcpClientSocket(pCallBackIntf),
m_pEncode(NULL),
m_pDecode(NULL),
m_pFnPtr(NULL)
//,m_dwLastSendTick(0),
//m_dwLastSendTick2(0)
{
	InitializeCriticalSection(&m_csLock);
}

CClientSocket::~CClientSocket()
{
	m_SocketFuncMap.clear();

	if (m_pEncode)
	{
		VirtualFree(m_pEncode, 0, MEM_RELEASE);
	}
	
	if (m_pDecode)
	{
		VirtualFree(m_pDecode, 0, MEM_RELEASE);
	}
	DeleteCriticalSection(&m_csLock);
}

BOOL CClientSocket::RegFunc(WORD wID, const MemberFuncInfo& info)
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

void CClientSocket::SetDbgCB(MemberFuncInfo& info)
{
	if (info.dwFuncAddr)
	{
		m_pFnPtr = info.dwFuncAddr;
		m_dwThis = info.dwThis;
	}
}

BOOL CClientSocket::Execute()
{
	BOOL bRet = CTcpClientSocket::Execute();

	CHAR *pData = NULL;
	INT nSize = 0;
	Head *pHead = NULL;
	while (m_RecvBuffer.Get(&pData, nSize))
	{
		pHead = (Head *)pData;
		switch(pHead->wIdent)
		{
		case SCM_ENCODE : 
			{
				m_pEncode = (TCodingProc)VirtualAlloc(NULL, nSize - sizeof(Head), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				memcpy(m_pEncode, pData + sizeof(Head), nSize - sizeof(Head));
				delete[] pData;
				continue;
			}
		case SCM_DECODE : 
			{
				m_pDecode = (TCodingProc)VirtualAlloc(NULL, nSize - sizeof(Head), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				memcpy(m_pDecode, pData + sizeof(Head), nSize - sizeof(Head));
				delete[] pData;
				continue;
			}
		}

		if (m_pDecode)
		{
			//OutputDebugStringW(Utils::FormatWStr(L"Decode Ident=%d, Len=%d\n", pHead->wIdent, pHead->wPackageLen).c_str());
			m_pDecode((BYTE *)(pHead) + ENCODE_START_LEN, nSize - ENCODE_START_LEN);
		}

		ProcessOneSocketpacket(pHead->wIdent, pData + sizeof(Head), nSize - sizeof(Head));
		delete[] pData;
	}

	return bRet;
}

void CClientSocket::ProcessOneSocketpacket(WORD wIdent, char* pBuf, int nLen)
{
	//OutputDebugStringW(Utils::FormatWStr(L"Packet Ident=%d, Len=%d\n", wIdent, nLen).c_str());

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

BOOL CClientSocket::Dispatch(WORD wID, char* pBuf, int nLen)
{
	#ifdef _GPROTECT
		if (wID == SCM_GPS_CHECK_REQUEST)
		{
			m_GPCheckThread.SetCheckData(pBuf, nLen);
			//g_pGProtect->ProcessResponse((unsigned char *)pBuf, nLen);
			//SendData(CM_GPS_CHECK_RESPONSE, pBuf, nLen);
			return TRUE;
		}
	#endif //_GPROTECT
	
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

			#if defined(INTERIOR) || defined(_DEBUG)
				if (m_pFnPtr)
				{
					CallMemberFunc(TRUE, m_pFnPtr, (void*)m_dwThis, 2, wID, nLen);
				}
			#endif
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
BOOL CClientSocket::SendData(WORD wID, LPCSTR pData /*= NULL*/, int nDataLen /*= 0*/)
{
#ifndef _NETWORK
	return TRUE;
#endif

//#ifdef _DEBUG
//	LogW::Trace(L"SendData:%d\n", wID);
//#endif

	EnterCriticalSection(&m_csLock);
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
		LeaveCriticalSection(&m_csLock);
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

	CHAR *pSendData = m_SendData.GetBuf();
	if (m_pEncode)
	{
		m_pEncode((BYTE *)pSendData + ENCODE_START_LEN, head.wPackageLen - ENCODE_START_LEN);
	}

	//#if (defined(_DEBUG) || defined(INTERIOR))
	//	if (wID != 10)
	//	{
	//		OutputDebugStringW(Utils::FormatWStr(L"Msg = %d TimeSpan %d [ %d ] \n", 
	//			wID, 
	//			timeGetTime() - m_dwLastSendTick, 
	//			_GTick - m_dwLastSendTick2).c_str());
	//		m_dwLastSendTick = timeGetTime();
	//		m_dwLastSendTick2 = _GTick;
	//	}
	//#endif

	BOOL bRet = SendBuf(pSendData, head.wPackageLen) > 0;
	LeaveCriticalSection(&m_csLock);
	return bRet;
}

void CClientSocket::OnRead( char *pBuf, INT nCount )
{
	CTcpClientSocket::OnRead(pBuf, nCount);
	if (nCount > 0)
	{
		m_RecvBuffer.Append(pBuf, nCount);
	}
}

void CClientSocket::OnDisconnet()
{
	CTcpClientSocket::OnDisconnet();
	m_RecvBuffer.Clear();
}