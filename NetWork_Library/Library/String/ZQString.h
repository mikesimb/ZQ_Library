#pragma once
#include <windows.h>
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#include <tchar.h>

struct CZQStringData
{
	long nRefs;//引用计数
	int nDataLength;//字符串使用长度
	int nAllocLength;//实际的内存分配长度
	char * data() { return (char*)(this + 1); };
	//this+1 相当与是CStringData[1];所以TCHAR* data()指的是CStringData[1]的地址
};

class CZQString
{
public:
	//构造函数
	CZQString();
	CZQString(const CZQString& stringSrc);
	CZQString(char ch, int nlength = 1);
	CZQString(char * lpsz);
	CZQString(char * lpsz, int length);
	CZQString(const unsigned char * psz);

	//析构函数
	virtual ~CZQString();

	//CZQStringData的属性

	int GetLength() const;//得到字符的长度
	int GetAllocLength() const;//得到内存分配的长度
	bool IsEmpty() const; //判断字符串长度是否为0

	void Empty();//清除CZQStringData;

	//操作符重载
	const CZQString& operator=(const CZQString& stringSrc);
	const CZQString& operator=(char * lpsz);
	const CZQString& operator=(char  ch);


	const CZQString& operator+=(const CZQString& stringSrc);
	const CZQString& operator+=(char * lpsz);
	const CZQString& operator+=(char  ch);

	operator char*() const; //类型转换

	char operator[](int nIndex) const;



	friend CZQString operator+(const CZQString& str1, const CZQString& str2);
	friend CZQString operator+(const CZQString& str1, char ch);
	friend CZQString operator+(char ch, const CZQString& str2);
	friend CZQString operator+(const CZQString& str1, char* lpsz);
	friend CZQString operator+(char* lpsz, const CZQString& str1);

	//基本的一些操作
	int Delete(int nIndex, int nCount = 1);//删除从nindex开始长度为你Count的数据
	int Insert(int nIndex, char ch);//插入一个字符
	int Insert(int nIndex, char* lpsz);//插入一个字符串
	int Replace(char * lpszOld, char * lpszNew);
	int Replace(char chrOld, char chrNew);
	int Remove(char chrRemove);
	void TrimRight(char * lpszTargetList);
	void TrimRight(char chTarget);
	void TrimRight();
	void TrimLeft(char * lpszTargets);
	void TrimLeft(char chrTarget);
	void TrimLeft();

	//获取某段字符串
	void SetAt(int nIndex, char chr);
	char GetAt(int nIndex) const;
	CZQString Mid(int nFirst) const;
	CZQString Mid(int nFirst, int nCount) const;
	CZQString Right(int nCount) const;
	CZQString Left(int nCount) const;
	void MakeUpper();
	void MakeLower();

	void MakeReverse();

	//查找
	int Find(char chr) const;
	int Find(char chr, int  nStart) const;
	int ReverseFind(char chr) const;
	int Find(char * lpszSub) const;
	int Find(char * lpszSub, int nStart) const;
	int FindOneOf(char * lpszCharset) const;//得到第一个匹配lpszCharSet中其中一个字符的位置 

	//其他的高级操作
	char * GetBuffer(int nMinBufLength);//重新分配内存，在COPY原来的数据
	void ReleaseBuffer(int nNewLength = -1); //在[nNewLength]='\0',对内存大小没有改变
	char * GetBufferSetLength(int nNewLength);//重新分配内存，然后COPY原来的数据
	void FreeExtra();
	char * LockBuffer();
	void UnLockBuffer();

	//比较
	int Compare(char * lpsz) const;
	int CompareNoCase(char* lpsz); //比较不区分大小写
	//比较速度没有Compare快
	int Collate(char * lpsz) const;
	int CollateNoCase(char * lpsz) const;//比较不区分大小写

	//格式化字符串
	void Format(char* lpsz, ...);

private:
	void Init();
	CZQStringData* GetData() const;
	void AllocBuffer(int nLen);
	void CopyBeforeWrite();//带引用计数的复制自己
	void AllocBeforeWrite(int nLen);
	void AssignCopy(int nSrcLen, char * lpszSrcData);

	//把nCopyIndex开始的nCopyLen长度的数据拷贝给dest,nExtraLen扩充的长度,次函数好像没下面用
	void AllocCopy(CZQString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void Release();
	void FormatV(char * lpszFormat, va_list argList);
	void ConcatCopy(int nSrc1len, char* lpszSrcData, int nSrc2Len, char* lpszSrcData2);//连接数据
	void ConcatInPlace(int nSrcLen, char * lpszSrcData);

	static void Release(CZQStringData * pData);
	static void FreeData(CZQStringData * pData);
	static int SafeStrlen(char * lpsz);

	char * m_pChData;//指向CStringData的数据区
};

					 /*调用CZQString::Compare比较大小,如果比较中有CStirng的话用
					 调用operator LPCTSTR()转化类型为LPCTSTR
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
//检测lpsz是否有效,调用了IsBadStringPtr
BOOL IsValidZQString(char * lpsz, int nLength);
//检测lp是否能读写权限,调用了IsBadReadPtr,IsBadStringPtr
BOOL IsValidAddress(const void* lp, UINT nBytes, BOOL bReadWrite = TRUE);

//CStirng数组操作
void ZQ_ConstructElements(CZQString* pElements, int nCount); //初始化CStirng数组
void ZQ_DestructElements(CZQString* pElements, int nCount); //删除CStirng数组
void CopyElements(CZQString* pDest, const CZQString* pSrc, int nCount); //CString数组拷贝

