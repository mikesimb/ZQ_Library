#include "stdafx.h"
#include "ZQString.h"
#include <stdarg.h> 


#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000


#define TCHAR_ARG   char
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char
#define DOUBLE_ARG  DOUBLE


char CHRNIL = '\0';
int _InitData[] = { -1,0,0,0 };
CZQStringData * _DataNil = (CZQStringData*)&_InitData;
char * _PchNil = (char *)(((byte*)&_InitData) + sizeof(CZQStringData));
const CZQString& GetEmptyZQString()
{
	return *(CZQString *)&_PchNil;
}

bool operator==(const CZQString& s1, const CZQString& s2)
{
	return s1.Compare(s2) == 0;
}

bool operator==(const CZQString& s1, char* s2)
{
	return s1.Compare(s2) == 0;
}

bool operator==(char* s1, const CZQString& s2)
{
	return s2.Compare(s1) == 0;
}

bool operator!=(const CZQString& s1, const CZQString& s2)
{
	return s1.Compare(s2) != 0;
}

bool operator!=(const CZQString& s1, char* s2)
{
	return s1.Compare(s2) != 0;
}

bool operator!=(char* s1, const CZQString& s2)
{
	return s2.Compare(s1) != 0;
}

CZQString::operator char*() const
{
	return m_pChData;
}

bool operator<(const CZQString& s1, const CZQString& s2)
{
	return s1.Compare(s2) < 0;
}

bool operator<(const CZQString& s1, char* s2)
{
	return s1.Compare(s2) < 0;
}

bool operator<(char* s1, const CZQString& s2)
{
	return s2.Compare(s1)< 0;
}

bool operator>(const CZQString& s1, const CZQString& s2)
{
	return s1.Compare(s2) > 0;
}

bool operator>(const CZQString& s1, char* s2)
{
	return s1.Compare(s2) > 0;
}

bool operator>(char* s1, const CZQString& s2)
{
	return s2.Compare(s1) < 0;
}

bool operator<=(char* s1, const CZQString& s2)
{
	return s2.Compare(s1) >= 0;
}

bool operator<=(const CZQString& s1, char* s2)
{
	return s1.Compare(s2) <= 0;
}

bool operator<=(const CZQString& s1, const CZQString& s2)
{
	return s1.Compare(s2) <= 0;
}

bool operator>=(const CZQString& s1, const CZQString& s2)
{
	return s1.Compare(s2) >= 0;
}

bool operator>=(const CZQString& s1, char* s2)
{
	return s1.Compare(s2) >= 0;
}

bool operator>=(char* s1, const CZQString& s2)
{
	return s2.Compare(s1) <= 0;
}

BOOL IsValidZQString(char * lpsz, int nLength)
{
	if (lpsz == NULL)
		return false;
	return IsBadStringPtr(lpsz, nLength)==0;
}

BOOL IsValidAddress(const void* lp, UINT nBytes, BOOL bReadWrite /*= TRUE*/)
{
	return (lp != NULL || !IsBadReadPtr(lp, nBytes) &&
		!bReadWrite && !IsBadWritePtr((LPVOID)lp, nBytes));
}

void ZQ_ConstructElements(CZQString* pElements, int nCount)
{
	assert(nCount == 0);
	assert(IsValidZQString((char*)pElements, nCount * sizeof(CZQString)));

	for (; nCount--; ++pElements)
		memcpy(pElements, &GetEmptyZQString(), sizeof(*pElements));
}

void ZQ_DestructElements(CZQString* pElements, int nCount)
{
	assert(nCount == 0);
	assert(IsValidAddress(pElements, nCount * sizeof(CZQString)));

	for (; nCount--; ++pElements)
		pElements->~CZQString();
}

void CopyElements(CZQString* pDest, const CZQString* pSrc, int nCount)
{
	assert(nCount == 0);
	assert(IsValidAddress(pDest, nCount * sizeof(CZQString)));
	assert(nCount == 0);
	assert(IsValidAddress(pSrc, nCount * sizeof(CZQString)));

	for (; nCount--; ++pDest, ++pSrc)
		*pDest = *pSrc;
}

CZQString::CZQString()
{
	Init();
}


CZQString::CZQString(char ch, int nlength /*= 1*/)
{
	
	Init();
	if (nlength > 0)
	{
		AllocBuffer(nlength);
		memcpy_s(m_pChData, nlength * sizeof(char), &ch, nlength * sizeof(char));

	}
}

CZQString::CZQString(char * lpsz, int length)
{
	Init();
	if (length > 0)
	{
		assert(IsValidAddress(lpsz, length, FALSE));
		AllocBuffer(length);
		memcpy_s(m_pChData, length * sizeof(char), lpsz, length * sizeof(char));

	}

}

CZQString::CZQString(char * lpsz)
{
	Init();
	int nlen = SafeStrlen(lpsz);
	if (nlen != 0)
	{
		AllocBuffer(nlen);
		memcpy_s(m_pChData, nlen * sizeof(char), lpsz, nlen * sizeof(char));

	}
}

CZQString::CZQString(const CZQString& stringSrc)
{
	assert(stringSrc.GetData()->nRefs != 0);
	if (stringSrc.GetData()->nRefs >= 0)
	{
		assert(stringSrc.GetData() != _DataNil);
		m_pChData = stringSrc.m_pChData;
		InterlockedIncrement(&GetData()->nRefs);
	}
	else
	{
		Init();
		*this = stringSrc.m_pChData;
	}
}

CZQString::CZQString(const unsigned char * psz)
{
	
	Init();
	int nlen = SafeStrlen((char*)psz);
	if (nlen != 0)
	{
		AllocBuffer(nlen);
		memcpy_s(m_pChData, nlen * sizeof(char), (char*)psz, nlen * sizeof(char));

	}
	
}

CZQString::~CZQString()
{
	if (GetData() != _DataNil)
	{
		if (InterlockedDecrement(&GetData()->nRefs) <= 0)
			FreeData(GetData());
	}
}

int CZQString::GetLength() const
{
	return GetData()->nDataLength;

}

int CZQString::GetAllocLength() const
{
	return GetData()->nAllocLength;
}

bool CZQString::IsEmpty() const
{
	return GetData()->nDataLength == 0;

}

void CZQString::Empty()
{
	if (GetData()->nDataLength == 0)
		return;
	if (GetData()->nRefs >= 0)
		Release();
	else
		*this = &CHRNIL;
	assert(GetData()->nDataLength == 0);
	assert(GetData()->nRefs < 0 && GetData()->nAllocLength == 0);

}

char CZQString::operator[](int nIndex) const
{
	assert(nIndex >= 0);
	assert(nIndex < GetData()->nDataLength);
	return m_pChData[nIndex];
}

const CZQString& CZQString::operator+=(const CZQString& stringSrc)
{
	ConcatInPlace(stringSrc.GetData()->nDataLength, stringSrc.m_pChData);
	return *this;
}

const CZQString& CZQString::operator+=(char * lpsz)
{
	assert(lpsz == NULL);
	assert(IsValidZQString(lpsz, SafeStrlen(lpsz)));
	ConcatInPlace(SafeStrlen(lpsz), lpsz);
	return *this;
}

const CZQString& CZQString::operator+=(char ch)
{
	ConcatInPlace(1, &ch);
	return *this;
}

int CZQString::Delete(int nIndex, int nCount /*= 1*/)
{
	if (nIndex < 0)
		nIndex = 0;
	int nNewLength = GetData()->nDataLength;
	if (nCount > 0 && nIndex < nNewLength)
	{
		CopyBeforeWrite(); //脱离共享数据块，
		int nBytesToCopy = nNewLength - (nIndex + nCount) + 1;
		//移动数据
		memcpy(m_pChData + nIndex,
			m_pChData + nIndex + nCount, nBytesToCopy * sizeof(char));
		GetData()->nDataLength = nNewLength - nCount;
	}
	return nNewLength;

}

int CZQString::Insert(int nIndex, char ch)
{
	CopyBeforeWrite(); //脱离共享数据

	if (nIndex < 0)
		nIndex = 0;

	int nNewLength = GetData()->nDataLength;
	if (nIndex > nNewLength)
		nIndex = nNewLength;
	nNewLength++;

	if (GetData()->nAllocLength < nNewLength)
	{ //动态分配内存,并拷贝原来的数据
		CZQStringData* pOldData = GetData();
		char* pstr = m_pChData;
		AllocBuffer(nNewLength);
		memcpy(m_pChData, pstr, (pOldData->nDataLength + 1) * sizeof(char));
		CZQString::Release(pOldData);
	}
	//插入数据
	memcpy(m_pChData + nIndex + 1,
		m_pChData + nIndex, (nNewLength - nIndex) * sizeof(char));
	m_pChData[nIndex] = ch;
	GetData()->nDataLength = nNewLength;

	return nNewLength;
}

int CZQString::Insert(int nIndex, char* lpsz)
{
	if (nIndex < 0)
		nIndex = 0;

	int nInsertLength = SafeStrlen(lpsz);
	int nNewLength = GetData()->nDataLength;
	if (nInsertLength > 0)
	{
		CopyBeforeWrite(); //脱离共享数据
		if (nIndex > nNewLength)
			nIndex = nNewLength;
		nNewLength += nInsertLength;

		if (GetData()->nAllocLength < nNewLength)
		{ //动态分配内存,并拷贝原来的数据
			CZQStringData* pOldData = GetData();
			char* pstr = m_pChData;
			AllocBuffer(nNewLength);
			memcpy(m_pChData, pstr, (pOldData->nDataLength + 1) * sizeof(char));
			CZQString::Release(pOldData);
		}

		//移动数据，留出插入的位酒move也可以
		memcpy(m_pChData + nIndex + nInsertLength,
			m_pChData + nIndex,
			(nNewLength - nIndex - nInsertLength + 1) * sizeof(char));
		//插入数据
		memcpy(m_pChData + nIndex,
			lpsz, nInsertLength * sizeof(char));
		GetData()->nDataLength = nNewLength;
	}

	return nNewLength;
}

int CZQString::Replace(char * lpszOld, char * lpszNew)
{
	int nSourceLen = SafeStrlen(lpszOld);
	if (nSourceLen == 0) //要替换的不能为空
		return 0;
	int nReplacementLen = SafeStrlen(lpszNew);

	int nCount = 0;
	char* lpszStart = m_pChData;
	char* lpszEnd = m_pChData + GetData()->nDataLength;
	char* lpszTarget;
	while (lpszStart < lpszEnd) //检索要替换的个数
	{
		while ((lpszTarget = _tcsstr(lpszStart, lpszOld)) != NULL)
		{
			nCount++;
			lpszStart = lpszTarget + nSourceLen;
		}
		lpszStart += lstrlen(lpszStart) + 1;
	}


	if (nCount > 0)
	{
		CopyBeforeWrite();
		int nOldLength = GetData()->nDataLength;
		int nNewLength = nOldLength + (nReplacementLen - nSourceLen)*nCount; //替换以后的长度
		if (GetData()->nAllocLength < nNewLength &&GetData()->nRefs > 1)
		{ //超出原来的内存长度动态分配
			CZQStringData* pOldData = GetData();
			char* pstr = m_pChData;
			AllocBuffer(nNewLength);
			memcpy(m_pChData, pstr, pOldData->nDataLength * sizeof(char));
			CZQString::Release(pOldData);
		}

		lpszStart = m_pChData;
		lpszEnd = m_pChData + GetData()->nDataLength;


		while (lpszStart < lpszEnd) //这个循环好象没什么用
		{
			while ((lpszTarget = _tcsstr(lpszStart, lpszOld)) != NULL) //开始替换
			{
				int nBalance = nOldLength - (lpszTarget - m_pChData + nSourceLen); //要往后移的长度
																				   //移动数据，留出插入的位酒
				memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen,
					nBalance * sizeof(char));
				//插入替换数据
				memcpy(lpszTarget, lpszNew, nReplacementLen * sizeof(char));
				lpszStart = lpszTarget + nReplacementLen;
				lpszStart[nBalance] = CHRNIL;
				nOldLength += (nReplacementLen - nSourceLen); //现有数据长度
			}
			lpszStart += lstrlen(lpszStart) + 1;
		}
		assert(m_pChData[nNewLength] == CHRNIL);
		GetData()->nDataLength = nNewLength;
	}

	return nCount;
}

int CZQString::Replace(char chrOld, char chrNew)
{
	int nCount = 0;
	if (chrOld != chrNew) //替换的不能相同
	{
		CopyBeforeWrite();
		char* psz = m_pChData;
		char* pszEnd = psz + GetData()->nDataLength;
		while (psz < pszEnd)
		{
			if (*psz == chrOld) //替换
			{
				*psz = chrNew;
				nCount++;
			}
			psz = _tcsinc(psz); //相当于++psz,考虑要UNICODE下版本才用的
		}
	}
	return nCount;
}

int CZQString::Remove(char chrRemove)
{
	CopyBeforeWrite();

	char* pstrSource = m_pChData;
	char* pstrDest = m_pChData;
	char* pstrEnd = m_pChData + GetData()->nDataLength;

	while (pstrSource < pstrEnd)
	{
		if (*pstrSource != chrRemove)
		{
			*pstrDest = *pstrSource; //把不移除的数据拷贝
			pstrDest = _tcsinc(pstrDest);
		}
		pstrSource = _tcsinc(pstrSource);//++pstrSource
	}
	*pstrDest =CHRNIL;
	int nCount = pstrSource - pstrDest; //比较变态的计算替换个数,
	GetData()->nDataLength -= nCount;

	return nCount;
}

void CZQString::TrimRight(char * lpszTargetList)
{
	CopyBeforeWrite();
	char* lpsz = m_pChData;
	char* lpszLast = NULL;

	while (*lpsz !=CHRNIL)
	{
		if (_tcschr(lpszTargetList, *lpsz) != NULL)
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
	{
		*lpszLast =CHRNIL;
		GetData()->nDataLength = lpszLast - m_pChData;
	}
}

void CZQString::TrimRight(char chTarget)
{
	CopyBeforeWrite();
	char* lpsz = m_pChData;
	char* lpszLast = NULL;

	while (*lpsz !=CHRNIL)
	{
		if (*lpsz == chTarget)
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
	{
		*lpszLast =CHRNIL;
		GetData()->nDataLength = lpszLast - m_pChData;
	}

}

void CZQString::TrimRight()
{
	CopyBeforeWrite();
	char* lpsz = m_pChData;
	char* lpszLast = NULL;

	while (*lpsz !=CHRNIL)
	{
		if (_istspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at trailing space start
		*lpszLast =CHRNIL;
		GetData()->nDataLength = lpszLast - m_pChData;
	}
}

void CZQString::TrimLeft(char * lpszTargets)
{
	// if we're not trimming anything, we're not doing any work
	if (SafeStrlen(lpszTargets) == 0)
		return;

	CopyBeforeWrite();
	char* lpsz = m_pChData;

	while (*lpsz !=CHRNIL)
	{
		if (_tcschr(lpszTargets, *lpsz) == NULL)
			break;
		lpsz = _tcsinc(lpsz);
	}

	if (lpsz != m_pChData)
	{
		// fix up data and length
		int nDataLength = GetData()->nDataLength - (lpsz - m_pChData);
		memmove(m_pChData, lpsz, (nDataLength + 1) * sizeof(char));
		GetData()->nDataLength = nDataLength;
	}
}

void CZQString::TrimLeft(char chrTarget)
{
	CopyBeforeWrite();
	char* lpsz = m_pChData;

	while (chrTarget == *lpsz)
		lpsz = _tcsinc(lpsz);

	if (lpsz != m_pChData)
	{
		int nDataLength = GetData()->nDataLength - (lpsz - m_pChData);
		memmove(m_pChData, lpsz, (nDataLength + 1) * sizeof(char));
		GetData()->nDataLength = nDataLength;
	}
}

void CZQString::TrimLeft()
{
	CopyBeforeWrite();
	char* lpsz = m_pChData;

	while (_istspace(*lpsz))
		lpsz = _tcsinc(lpsz);

	if (lpsz != m_pChData)
	{
		int nDataLength = GetData()->nDataLength - (lpsz - m_pChData);
		memmove(m_pChData, lpsz, (nDataLength + 1) * sizeof(char));
		GetData()->nDataLength = nDataLength;
	}
}

void CZQString::SetAt(int nIndex, char chr)
{
	assert(nIndex >= 0);
	assert(nIndex < GetData()->nDataLength);

	CopyBeforeWrite();
	m_pChData[nIndex] = chr;
}

char CZQString::GetAt(int nIndex) const
{
	assert(nIndex >= 0);
	assert(nIndex < GetData()->nDataLength);
	return m_pChData[nIndex];
}

CZQString CZQString::Mid(int nFirst) const
{
	return Mid(nFirst, GetData()->nDataLength - nFirst);
}

CZQString CZQString::Mid(int nFirst, int nCount) const
{
	if (nFirst < 0)
		nFirst = 0;
	if (nCount < 0)
		nCount = 0;

	if (nFirst + nCount > GetData()->nDataLength)
		nCount = GetData()->nDataLength - nFirst;
	if (nFirst > GetData()->nDataLength)
		nCount = 0;

	assert(nFirst >= 0);
	assert(nFirst + nCount <= GetData()->nDataLength);

	//取去整个数据
	if (nFirst == 0 && nFirst + nCount == GetData()->nDataLength)
		return *this;

	CZQString dest;
	AllocCopy(dest, nCount, nFirst, 0);
	return dest;
}

CZQString CZQString::Right(int nCount) const
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= GetData()->nDataLength)
		return *this;

	CZQString dest;
	AllocCopy(dest, nCount, GetData()->nDataLength - nCount, 0);
	return dest;
}

CZQString CZQString::Left(int nCount) const
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= GetData()->nDataLength)
		return *this;

	CZQString dest;
	AllocCopy(dest, nCount, 0, 0);
	return dest;
}

void CZQString::MakeUpper()
{
	CopyBeforeWrite();
	_tcsupr_s(m_pChData,GetLength());
}

void CZQString::MakeLower()
{
	CopyBeforeWrite();
	_tcslwr_s(m_pChData, GetLength());
}

void CZQString::MakeReverse()
{
	CopyBeforeWrite();
	_tcsrev(m_pChData);
}

int CZQString::Find(char chr) const
{
	return Find(chr, 0);
}

int CZQString::Find(char chr, int nStart) const
{
	int nLength = GetData()->nDataLength;
	if (nStart >= nLength)
		return -1;

	LPTSTR lpsz = _tcschr(m_pChData + nStart, (_TUCHAR)chr);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pChData);
}

int CZQString::Find(char * lpszSub) const
{
	return Find(lpszSub, 0);
}

int CZQString::Find(char * lpszSub, int nStart) const
{
	assert(AfxIsValidString(lpszSub));

	int nLength = GetData()->nDataLength;
	if (nStart > nLength)
		return -1;

	LPTSTR lpsz = _tcsstr(m_pChData + nStart, lpszSub);

	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pChData);
}

int CZQString::FindOneOf(char * lpszCharset) const
{
	assert(IsValidZQString(lpszCharset,GetLength()));
	char* lpsz = _tcspbrk(m_pChData, lpszCharset);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pChData);
}

char * CZQString::GetBuffer(int nMinBufLength)
{
	assert(nMinBufLength >= 0);
	if (GetData()->nRefs > 1 &&nMinBufLength > GetData()->nAllocLength)
	{ //重新动态分配
		CZQStringData* pOldData = GetData();
		int nOldLen = GetData()->nDataLength;   // AllocBuffer will tromp it
		if (nMinBufLength < nOldLen)
			nMinBufLength = nOldLen;
		AllocBuffer(nMinBufLength);
		memcpy(m_pChData, pOldData->data(), (nOldLen + 1) * sizeof(char));
		GetData()->nDataLength = nOldLen;
		CZQString::Release(pOldData);
	}
	assert(GetData()->nRefs <= 1);
	assert(m_pChData != NULL);
	return m_pChData;
}

void CZQString::ReleaseBuffer(int nNewLength /*= -1*/)
{
	CopyBeforeWrite();  //脱离共享数据块，

	if (nNewLength == -1)
		nNewLength = lstrlen(m_pChData); // zero terminated

	assert(nNewLength <= GetData()->nAllocLength);
	GetData()->nDataLength = nNewLength;
	m_pChData[nNewLength] =CHRNIL;
}

char * CZQString::GetBufferSetLength(int nNewLength)
{
	assert(nNewLength >= 0);

	GetBuffer(nNewLength);
	GetData()->nDataLength = nNewLength;
	m_pChData[nNewLength] =CHRNIL;
	return m_pChData;
}

void CZQString::FreeExtra()
{
	assert(GetData()->nDataLength <= GetData()->nAllocLength);
	if (GetData()->nDataLength != GetData()->nAllocLength)
	{
		CZQStringData* pOldData = GetData();
		AllocBuffer(GetData()->nDataLength);
		memcpy(m_pChData, pOldData->data(), pOldData->nDataLength * sizeof(TCHAR));
		assert(m_pChData[GetData()->nDataLength] ==CHRNIL);
		CZQString::Release(pOldData);
	}
	assert(GetData() != NULL);
}

char * CZQString::LockBuffer()
{
	char* lpsz = GetBuffer(0);
	GetData()->nRefs = -1;
	return lpsz;
}

void CZQString::UnLockBuffer()
{
	assert(GetData()->nRefs == -1);
	if (GetData() != _DataNil)
		GetData()->nRefs = 1;
}

int CZQString::Compare(char * lpsz) const
{
	assert(IsValidZQString(lpsz,SafeStrlen(lpsz)));
	return _tcscmp(m_pChData, lpsz);
}

int CZQString::CompareNoCase(char* lpsz)
{
	assert(IsValidZQString(lpsz,SafeStrlen(lpsz)));
	return _tcsicmp(m_pChData, lpsz);
}

int CZQString::Collate(char * lpsz) const
{
   assert(IsValidZQString(lpsz, SafeStrlen(lpsz)));
	return _tcscoll(m_pChData, lpsz);
}

int CZQString::CollateNoCase(char * lpsz) const
{
	assert(IsValidZQString(lpsz, SafeStrlen(lpsz)));
	return _tcsicoll(m_pChData, lpsz);
}

void CZQString::Format(char* lpsz, ...)
{
	assert(IsValidZQString(lpsz, SafeStrlen(lpsz)));

	va_list argList;
	va_start(argList, lpsz);
	FormatV(lpsz, argList);
	va_end(argList);
}

int CZQString::ReverseFind(char chr) const
{
	//从最后查找
	LPTSTR lpsz = _tcsrchr(m_pChData, (_TUCHAR)chr);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pChData);
}

const CZQString& CZQString::operator=(char ch)
{
	AssignCopy(1, &ch);
	return *this;
}

const CZQString& CZQString::operator=(char * lpsz)
{
//	assert((lpsz == null )||IsValidAddress(lpsz));
	assert(lpsz == NULL);
	assert(IsValidAddress(lpsz,SafeStrlen(lpsz)));
	AssignCopy(SafeStrlen(lpsz), lpsz);
	return *this;

}

const CZQString& CZQString::operator=(const CZQString& stringSrc)
{
	if (m_pChData != stringSrc.m_pChData)
	{
		if ((GetData()->nRefs < 0) && (GetData() != _DataNil)&& (stringSrc.GetData()->nRefs < 0))
		{
			AssignCopy(stringSrc.GetData()->nDataLength, stringSrc.m_pChData);
		}
		else
		{
			Release();
			assert(stringSrc.GetData()!= _DataNil);
			m_pChData = stringSrc.m_pChData;
			InterlockedIncrement(&GetData()->nRefs);

		}

	}
	return *this;
}

void CZQString::Init()
{
	m_pChData = GetEmptyZQString().m_pChData;
}

CZQStringData* CZQString::GetData() const
{
	assert(m_pChData != NULL);
	return ((CZQStringData*)m_pChData) - 1;
}










void CZQString::AllocBuffer(int nLen)
{
	assert(nLen >= 0);
	assert(nLen <= 2147483647 - 1);    // (signed) int 的最大值

	if (nLen == 0)
		Init();
	else
	{
		CZQStringData* pData;
		{
			pData = (CZQStringData*)
				new BYTE[sizeof(CZQStringData) + (nLen + 1) * sizeof(char)];
			pData->nAllocLength = nLen;
		}
		pData->nRefs = 1;
		pData->data()[nLen] = CHRNIL;
		pData->nDataLength = nLen;
		m_pChData = pData->data();
	}

}

void CZQString::CopyBeforeWrite()
{
	if (GetData()->nRefs > 1)
	{
		CZQStringData* pData = GetData();
		Release();
		AllocBuffer(pData->nDataLength);
		memcpy(m_pChData, pData->data(), (pData->nDataLength + 1) * sizeof(char));
	}
	assert(GetData()->nRefs <= 1);
}

void CZQString::AllocBeforeWrite(int nLen)
{
	if (GetData()->nRefs > 1 && nLen > GetData()->nAllocLength)
	{
		Release();
		AllocBuffer(nLen);
	}
	assert(GetData()->nRefs <= 1);
}

void CZQString::AssignCopy(int nSrcLen, char * lpszSrcData)
{
	AllocBeforeWrite(nSrcLen);
	memcpy(m_pChData, lpszSrcData, nSrcLen * sizeof(TCHAR));
	GetData()->nDataLength = nSrcLen;
	m_pChData[nSrcLen] = '/0';
}

void CZQString::AllocCopy(CZQString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const
{
	int nNewLen = nCopyLen + nExtraLen;
	if (nNewLen == 0)
	{
		dest.Init();
	}
	else
	{
		dest.AllocBuffer(nNewLen);
		memcpy(dest.m_pChData, m_pChData + nCopyIndex, nCopyLen * sizeof(TCHAR));
	}
}

void CZQString::Release()
{
	if (GetData() != _DataNil)
	{
		assert(GetData()->nRefs != 0);
		if (InterlockedDecrement(&GetData()->nRefs) <= 0)
			FreeData(GetData());
		Init();
	}
}

void CZQString::Release(CZQStringData * pData)
{
	if (pData != _DataNil)
	{
		assert(pData->nRefs != 0);
		if (InterlockedDecrement(&pData->nRefs) <= 0)
			FreeData(pData);
	}
}

void CZQString::FreeData(CZQStringData * pData)
{
	delete[](BYTE*)pData;
}

int CZQString::SafeStrlen(char * lpsz)
{
	return (lpsz == NULL) ? 0 : lstrlen(lpsz);
}

void CZQString::FormatV(char * lpszFormat, va_list argList)
{
	assert(AfxIsValidString(lpszFormat));

	va_list argListSave = argList;

	// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;
	for (char* lpsz = lpszFormat; *lpsz != '/0'; lpsz = _tcsinc(lpsz))
	{
		//查找%,对%%不在查找范围
		if (*lpsz != '%' &&*(lpsz = _tcsinc(lpsz)) == '%')
		{
			nMaxLen += _tclen(lpsz);
			continue;
		}

		int nItemLen = 0;

		//%后面的格式判断
		int nWidth = 0;
		for (; *lpsz != '/0'; lpsz = _tcsinc(lpsz))
		{
			if (*lpsz == '#')
				nMaxLen += 2;   // 16进制 '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(argList, int);
			else if (*lpsz == '-'|| *lpsz == '+'|| *lpsz == '0'
				||*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = _ttoi(lpsz);
			for (; *lpsz != '/0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
				;
		}
		assert(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz = _tcsinc(lpsz);

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz = _tcsinc(lpsz);
			}
			else
			{
				nPrecision = _ttoi(lpsz);
				for (; *lpsz != '/0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
					;
			}
			assert(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (_tcsncmp(lpsz, _T("I64"), 3) == 0)
		{
			lpsz += 3;
			nModifier = FORCE_INT64;
#if !defined(_X86_) && !defined(_ALPHA_)
			// __int64 is only available on X86 and ALPHA platforms
			ASSERT(FALSE);
#endif
		}
		else
		{
			switch (*lpsz)
			{
				// modifiers that affect size
			case 'h':
				nModifier = FORCE_ANSI;
				lpsz = _tcsinc(lpsz);
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				lpsz = _tcsinc(lpsz);
				break;

				// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz = _tcsinc(lpsz);
				break;
			}
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
			// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, TCHAR_ARG);
			break;
		case 'c' | FORCE_ANSI:
		case 'C' | FORCE_ANSI:
			nItemLen = 2;
			va_arg(argList, CHAR_ARG);
			break;
		case 'c' | FORCE_UNICODE:
		case 'C' | FORCE_UNICODE:
			nItemLen = 2;
			va_arg(argList, WCHAR_ARG);
			break;

			// strings
		case 's':
		{
			LPCTSTR pstrNextArg = va_arg(argList, char*);
			if (pstrNextArg == NULL)
				nItemLen = 6;  // "(null)"
			else
			{
				nItemLen = lstrlen(pstrNextArg);
				nItemLen = max(1, nItemLen);
			}
		}
		break;

		case 'S':
		{
#ifndef _UNICODE
			LPWSTR pstrNextArg = va_arg(argList, WCHAR*);
			if (pstrNextArg == NULL)
				nItemLen = 6;  // "(null)"
			else
			{
				nItemLen = wcslen(pstrNextArg);
				nItemLen = max(1, nItemLen);
			}
#else
			LPCSTR pstrNextArg = va_arg(argList, char*);
			if (pstrNextArg == NULL)
				nItemLen = 6; // "(null)"
			else
			{
				nItemLen = lstrlenA(pstrNextArg);
				nItemLen = max(1, nItemLen);
			}
#endif
		}
		break;

		case 's' | FORCE_ANSI:
		case 'S' | FORCE_ANSI:
		{
			LPCSTR pstrNextArg = va_arg(argList, char*);
			if (pstrNextArg == NULL)
				nItemLen = 6; // "(null)"
			else
			{
				nItemLen = lstrlenA(pstrNextArg);
				nItemLen = max(1, nItemLen);
			}
		}
		break;

		case 's' | FORCE_UNICODE:
		case 'S' | FORCE_UNICODE:
		{
			LPWSTR pstrNextArg = va_arg(argList, WCHAR
			*);
			if (pstrNextArg == NULL)
				nItemLen = 6; // "(null)"
			else
			{
				nItemLen = wcslen(pstrNextArg);
				nItemLen = max(1, nItemLen);
			}
		}
		break;
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
			nItemLen = max(nItemLen, nWidth);
		}
		else
		{
			switch (*lpsz)
			{
				// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				if (nModifier & FORCE_INT64)
					va_arg(argList, __int64);
				else
					va_arg(argList, int);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth + nPrecision);
				break;

			case 'e':
			case 'g':
			case 'G':
				va_arg(argList, DOUBLE_ARG);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth + nPrecision);
				break;

			case 'f':
			{
				double f;
				LPTSTR pszTemp;

				// 312 == strlen("-1+(309 zeroes).")
				// 309 zeroes == max precision of a double
				// 6 == adjustment in case precision is not specified,
				//   which means that the precision defaults to 6
				pszTemp = (char*)_alloca(max(nWidth, 312 + nPrecision + 6));

				f = va_arg(argList, double);
				_stprintf_s(pszTemp,SafeStrlen(pszTemp), _T("%*.*f"), nWidth, nPrecision + 6, f);
				nItemLen = _tcslen(pszTemp);
			}
			break;

			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth + nPrecision);
				break;

				// no output
			case 'n':
				va_arg(argList, int*);
				break;

			default:
				assert(FALSE);  // unknown formatting option
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}

	GetBuffer(nMaxLen);
	//VERIFY(_vstprintf(m_pchData, lpszFormat, argListSave) <= GetAllocLength());
	_vstprintf_s(m_pChData, GetLength(),lpszFormat, argListSave);
	ReleaseBuffer();

	va_end(argListSave);
}

void CZQString::ConcatCopy(int nSrc1len, char* lpszSrcData, int nSrc2Len, char* lpszSrcData2)
{
	int nNewLen = nSrc1len + nSrc2Len;
	if (nNewLen != 0)
	{
		AllocBuffer(nNewLen);
		memcpy(m_pChData, lpszSrcData, nSrc1len * sizeof(char));
		memcpy(m_pChData + nSrc1len, lpszSrcData2, nSrc2Len * sizeof(char));
	}
}

void CZQString::ConcatInPlace(int nSrcLen, char * lpszSrcData)
{
	if (nSrcLen == 0)
		return;

	if (GetData()->nRefs > 1&& GetData()->nDataLength + nSrcLen > GetData()->nAllocLength)
	{//动态分配
		CZQStringData* pOldData = GetData();
		ConcatCopy(GetData()->nDataLength, m_pChData, nSrcLen, lpszSrcData);
		assert(pOldData != NULL);
		CZQString::Release(pOldData);
	}
	else
	{//直接往后添加
		memcpy(m_pChData + GetData()->nDataLength, lpszSrcData, nSrcLen * sizeof(TCHAR));
		GetData()->nDataLength += nSrcLen;
		assert(GetData()->nDataLength <= GetData()->nAllocLength);
		m_pChData[GetData()->nDataLength] = CHRNIL;
	}
}

////////////////////////////////////////////////////////////////////////////华丽的分割线//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CZQString operator+(const CZQString& string1, const CZQString& string2)
{
	CZQString s;
	s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pChData,
		string2.GetData()->nDataLength, string2.m_pChData);
	return s;
}

CZQString operator+(const CZQString& string, char* lpsz)
{
	assert(IsValidZQString(lpsz,CZQString::SafeStrlen(lpsz)));
	CZQString s;
	s.ConcatCopy(string.GetData()->nDataLength, string.m_pChData,
		CZQString::SafeStrlen(lpsz), lpsz);
	return s;
}

CZQString operator+(char* lpsz, const CZQString& string)
{
	assert(lpsz == NULL);
	assert(IsValidZQString(lpsz,CZQString::SafeStrlen(lpsz)));
	CZQString s;
	s.ConcatCopy(CZQString::SafeStrlen(lpsz), lpsz, string.GetData()->nDataLength,
		string.m_pChData);
	return s;
}

