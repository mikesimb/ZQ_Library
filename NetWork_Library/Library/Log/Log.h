#pragma once


class CLog
{
public:
	CLog();
	virtual ~CLog();
	static CLog* getInstance();
	static void FreeInstance();
	void InitializeLogFile();//���������Ŀ�ľ���ȷ����־�ļ����ļ�����
	void setLogFilename(char* pFilename);
	bool OpenFile();
	bool FileIsOpen();
	void WriteLog(LPVOID lpBuffer,DWORD dwLength);
private:
	//�ļ�����
	char * m_szFileName;
	//�ļ����
	HANDLE m_hFile;

	
};

