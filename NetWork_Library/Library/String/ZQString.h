#pragma once
#include <windows.h>
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#include <tchar.h>

struct CZQStringData
{
	long nRefs;//���ü���
	int nDataLength;//�ַ���ʹ�ó���
	int nAllocLength;//ʵ�ʵ��ڴ���䳤��
	char * data() { return (char*)(this + 1); };
	//this+1 �൱����CStringData[1];����TCHAR* data()ָ����CStringData[1]�ĵ�ַ
};

class CZQString
{
public:
	//���캯��
	CZQString();
	CZQString(const CZQString& stringSrc);
	CZQString(char ch, int nlength = 1);
	CZQString(char * lpsz);
	CZQString(char * lpsz, int length);
	CZQString(const unsigned char * psz);

	//��������
	virtual ~CZQString();

	//CZQStringData������

	int GetLength() const;//�õ��ַ��ĳ���
	int GetAllocLength() const;//�õ��ڴ����ĳ���
	bool IsEmpty() const; //�ж��ַ��������Ƿ�Ϊ0

	void Empty();//���CZQStringData;

	//����������
	const CZQString& operator=(const CZQString& stringSrc);
	const CZQString& operator=(char * lpsz);
	const CZQString& operator=(char  ch);


	const CZQString& operator+=(const CZQString& stringSrc);
	const CZQString& operator+=(char * lpsz);
	const CZQString& operator+=(char  ch);

	operator char*() const; //����ת��

	char operator[](int nIndex) const;



	friend CZQString operator+(const CZQString& str1, const CZQString& str2);
	friend CZQString operator+(const CZQString& str1, char ch);
	friend CZQString operator+(char ch, const CZQString& str2);
	friend CZQString operator+(const CZQString& str1, char* lpsz);
	friend CZQString operator+(char* lpsz, const CZQString& str1);

	//������һЩ����
	int Delete(int nIndex, int nCount = 1);//ɾ����nindex��ʼ����Ϊ��Count������
	int Insert(int nIndex, char ch);//����һ���ַ�
	int Insert(int nIndex, char* lpsz);//����һ���ַ���
	int Replace(char * lpszOld, char * lpszNew);
	int Replace(char chrOld, char chrNew);
	int Remove(char chrRemove);
	void TrimRight(char * lpszTargetList);
	void TrimRight(char chTarget);
	void TrimRight();
	void TrimLeft(char * lpszTargets);
	void TrimLeft(char chrTarget);
	void TrimLeft();

	//��ȡĳ���ַ���
	void SetAt(int nIndex, char chr);
	char GetAt(int nIndex) const;
	CZQString Mid(int nFirst) const;
	CZQString Mid(int nFirst, int nCount) const;
	CZQString Right(int nCount) const;
	CZQString Left(int nCount) const;
	void MakeUpper();
	void MakeLower();

	void MakeReverse();

	//����
	int Find(char chr) const;
	int Find(char chr, int  nStart) const;
	int ReverseFind(char chr) const;
	int Find(char * lpszSub) const;
	int Find(char * lpszSub, int nStart) const;
	int FindOneOf(char * lpszCharset) const;//�õ���һ��ƥ��lpszCharSet������һ���ַ���λ�� 

	//�����ĸ߼�����
	char * GetBuffer(int nMinBufLength);//���·����ڴ棬��COPYԭ��������
	void ReleaseBuffer(int nNewLength = -1); //��[nNewLength]='\0',���ڴ��Сû�иı�
	char * GetBufferSetLength(int nNewLength);//���·����ڴ棬Ȼ��COPYԭ��������
	void FreeExtra();
	char * LockBuffer();
	void UnLockBuffer();

	//�Ƚ�
	int Compare(char * lpsz) const;
	int CompareNoCase(char* lpsz); //�Ƚϲ����ִ�Сд
	//�Ƚ��ٶ�û��Compare��
	int Collate(char * lpsz) const;
	int CollateNoCase(char * lpsz) const;//�Ƚϲ����ִ�Сд

	//��ʽ���ַ���
	void Format(char* lpsz, ...);

private:
	void Init();
	CZQStringData* GetData() const;
	void AllocBuffer(int nLen);
	void CopyBeforeWrite();//�����ü����ĸ����Լ�
	void AllocBeforeWrite(int nLen);
	void AssignCopy(int nSrcLen, char * lpszSrcData);

	//��nCopyIndex��ʼ��nCopyLen���ȵ����ݿ�����dest,nExtraLen����ĳ���,�κ�������û������
	void AllocCopy(CZQString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void Release();
	void FormatV(char * lpszFormat, va_list argList);
	void ConcatCopy(int nSrc1len, char* lpszSrcData, int nSrc2Len, char* lpszSrcData2);//��������
	void ConcatInPlace(int nSrcLen, char * lpszSrcData);

	static void Release(CZQStringData * pData);
	static void FreeData(CZQStringData * pData);
	static int SafeStrlen(char * lpsz);

	char * m_pChData;//ָ��CStringData��������
};

					 /*����CZQString::Compare�Ƚϴ�С,����Ƚ�����CStirng�Ļ���
					 ����operator LPCTSTR()ת������ΪLPCTSTR
					 */
bool operator==(const CZQString& s1, const CZQString& s2);
bool operator==(const CZQString& s1, char* s2);
bool operator==(char* s1, const CZQString& s2);
bool operator!=(const CZQString& s1, const CZQString& s2);
bool operator!=(const CZQString& s1, char* s2);
bool operator!=(char* s1, const CZQString& s2);
bool operator<(const CZQString& s1, const CZQString& s2);
bool operator<(const CZQString& s1, char* s2);
bool operator<(char* s1, const CZQString& s2);
bool operator>(const CZQString& s1, const CZQString& s2);
bool operator>(const CZQString& s1, char* s2);
bool operator>(char* s1, const CZQString& s2);
bool operator<=(const CZQString& s1, const CZQString& s2);
bool operator<=(const CZQString& s1, char* s2);
bool operator<=(char* s1, const CZQString& s2);
bool operator>=(const CZQString& s1, const CZQString& s2);
bool operator>=(const CZQString& s1, char* s2);
bool operator>=(char* s1, const CZQString& s2);

//////////////////////////////////////////////////////////////////////
//���lpsz�Ƿ���Ч,������IsBadStringPtr
BOOL IsValidZQString(char * lpsz, int nLength);
//���lp�Ƿ��ܶ�дȨ��,������IsBadReadPtr,IsBadStringPtr
BOOL IsValidAddress(const void* lp, UINT nBytes, BOOL bReadWrite = TRUE);

//CStirng�������
void ZQ_ConstructElements(CZQString* pElements, int nCount); //��ʼ��CStirng����
void ZQ_DestructElements(CZQString* pElements, int nCount); //ɾ��CStirng����
void CopyElements(CZQString* pDest, const CZQString* pSrc, int nCount); //CString���鿽��

