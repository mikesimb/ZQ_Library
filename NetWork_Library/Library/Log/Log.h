#pragma once


class CLog
{
public:
	CLog();
	virtual ~CLog();
	static CLog* getInstance();
	static void FreeInstance();
	void InitializeLogFile();//这个函数的目的就是确定日志文件的文件名称
	void setLogFilename(char* pFilename);
	bool OpenFile();
	bool FileIsOpen();
	void WriteLog(LPVOID lpBuffer,DWORD dwLength);
private:
	//文件名称
	char * m_szFileName;
	//文件句柄
	HANDLE m_hFile;

	
};

